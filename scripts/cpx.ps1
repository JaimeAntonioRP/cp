# ---------------------------------------------------------------------------
#  cpx - CLI de programacion competitiva
#  Uso:  cpx <comando> [args]        (ver `cpx help`)
# ---------------------------------------------------------------------------
# Sin [CmdletBinding()] ni [Parameter()] a proposito: con el binder avanzado,
# PowerShell reserva los parametros comunes y `-In` se vuelve ambiguo con
# -InformationAction / -InformationVariable. Con un param() vacio, todo llega
# crudo en $args y lo parseamos nosotros.
param()

$argv    = @($args)
$Command = if ($argv.Count -gt 0) { [string]$argv[0] } else { 'help' }
$Rest    = if ($argv.Count -gt 1) { [string[]]$argv[1..($argv.Count - 1)] } else { @() }

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'lib\common.ps1')

$ROOT = Get-CpRoot
$CFG  = Get-CpConfig

# ---------------------------------------------------------------- utilidades
function Split-Args {
    param([string[]]$Argv, [string[]]$Switches = @())
    $pos = @(); $flags = @{}
    for ($i = 0; $i -lt $Argv.Count; $i++) {
        $a = $Argv[$i]
        if ($a -match '^-{1,2}[A-Za-z]') {
            $key = $a -replace '^-{1,2}', ''
            if ($Switches -contains $key) {
                $flags[$key] = $true
            } else {
                $i++
                $flags[$key] = if ($i -lt $Argv.Count) { $Argv[$i] } else { '' }
            }
        } else {
            $pos += $a
        }
    }
    return [pscustomobject]@{ Pos = $pos; Flags = $flags }
}

function Get-Flag($parsed, [string]$name, $default = $null) {
    if ($parsed.Flags.ContainsKey($name)) { return $parsed.Flags[$name] }
    return $default
}

function New-ProblemDir {
    param([string]$Dir, [int]$Tests = 1, [string]$Template = 'main.cpp')
    if (-not (Test-Path $Dir)) { New-Item -ItemType Directory -Path $Dir -Force | Out-Null }

    $dst = Join-Path $Dir 'main.cpp'
    if (-not (Test-Path $dst)) {
        Copy-Item (Join-Path $ROOT "templates\$Template") $dst
    }
    $testDir = Join-Path $Dir 'tests'
    if (-not (Test-Path $testDir)) { New-Item -ItemType Directory -Path $testDir -Force | Out-Null }
    for ($i = 1; $i -le $Tests; $i++) {
        foreach ($ext in @('in', 'out')) {
            $f = Join-Path $testDir "$i.$ext"
            if (-not (Test-Path $f)) { Write-TextFile -Path $f }
        }
    }
}

function Expand-ProblemList([string]$spec) {
    if (-not $spec) { return @('A') }
    if ($spec -match '^\d+$') {
        return 0..([int]$spec - 1) | ForEach-Object { [char](65 + $_) } | ForEach-Object { "$_" }
    }
    if ($spec -match '^([A-Za-z])\s*-\s*([A-Za-z])$') {
        $a = [int][char]$Matches[1].ToUpper(); $b = [int][char]$Matches[2].ToUpper()
        return $a..$b | ForEach-Object { "$([char]$_)" }
    }
    return ($spec -split '[,\s]+' | Where-Object { $_ })
}

function Open-InEditor([string[]]$Paths) {
    $ed = Get-Command $CFG.editor -ErrorAction SilentlyContinue
    if (-not $ed) { return }
    & $ed.Source @Paths | Out-Null
}

# =========================================================== comandos =======

function Cmd-New {
    $p = Split-Args $Rest -Switches @('Open')
    if ($p.Pos.Count -lt 1) { Fail 'Uso: cpx new <nombre> [-Tests n] [-Url ...] [-Open]'; return }

    $name = $p.Pos[0]
    $dir = if ([IO.Path]::IsPathRooted($name) -or $name -match '[\\/]') {
        (Join-Path $ROOT $name)
    } else {
        (Join-Path (Join-Path $ROOT 'practice') $name)
    }

    New-ProblemDir -Dir $dir -Tests ([int](Get-Flag $p 'Tests' 1))

    $url = Get-Flag $p 'Url' ''
    if ($url) {
        $src = Join-Path $dir 'main.cpp'
        $txt = Get-Content $src -Raw -Encoding UTF8
        Write-TextFile -Path $src -Content ($txt -replace ' \*  Enlace  :', " *  Enlace  : $url")
    }

    Ok "Creado: $dir"
    Dim "  main.cpp + tests/1.in + tests/1.out"
    Dim "  siguiente:  cd `"$dir`"  &&  cpx test"
    if (Get-Flag $p 'Open' $false) { Open-InEditor @($dir, (Join-Path $dir 'main.cpp')) }
}

function Cmd-Contest {
    $p = Split-Args $Rest -Switches @('Open', 'Brute')
    if ($p.Pos.Count -lt 1) {
        Fail 'Uso: cpx contest <slug> [-Problems A-F | 6 | A,B,C] [-Judge cf] [-Url ...] [-Open]'
        return
    }
    $slug  = $p.Pos[0]
    $probs = Expand-ProblemList (Get-Flag $p 'Problems' 'A-F')
    $judge = Get-Flag $p 'Judge' $CFG.defaultJudge
    $date  = Get-Date -Format 'yyyy-MM-dd'
    $dir   = Join-Path (Join-Path $ROOT 'contests') "$date-$slug"

    foreach ($x in $probs) {
        New-ProblemDir -Dir (Join-Path $dir $x) -Tests 1
        if (Get-Flag $p 'Brute' $false) {
            $b = Join-Path (Join-Path $dir $x) 'brute.cpp'
            if (-not (Test-Path $b)) { Copy-Item (Join-Path $ROOT 'templates\brute.cpp') $b }
        }
    }

    $meta = [ordered]@{
        slug     = $slug
        judge    = $judge
        date     = $date
        url      = (Get-Flag $p 'Url' '')
        problems = @($probs)
    }
    Write-TextFile (Join-Path $dir 'contest.json') ($meta | ConvertTo-Json -Depth 4)

    $md = @("# $slug", '', "Juez: $judge   Fecha: $date", '')
    foreach ($x in $probs) { $md += "- [ ] **$x** - " }
    Write-TextFile (Join-Path $dir 'README.md') (($md -join "`r`n") + "`r`n")

    Ok "Concurso creado: $dir"
    Dim ("  problemas: " + ($probs -join ' '))
    Dim "  siguiente:  cd `"$dir\$($probs[0])`"  &&  cpx test"
    if (Get-Flag $p 'Open' $false) { Open-InEditor @($dir) }
}

function Cmd-Run {
    $p = Split-Args $Rest -Switches @('Release', 'San')
    $prob = Resolve-Problem ($p.Pos | Select-Object -First 1)
    $exe = Get-ExePath $prob.Dir $prob.Name

    Info "Compilando $($prob.Name).cpp ..."
    if (-not (Invoke-Build -Source $prob.Source -Exe $exe `
                -Release:([bool](Get-Flag $p 'Release' $false)) `
                -San:([bool](Get-Flag $p 'San' $false)))) { return }

    $inFile = Get-Flag $p 'In' ''
    if ($inFile) {
        if (-not [IO.Path]::IsPathRooted($inFile)) { $inFile = Join-Path $prob.Dir $inFile }
        if (-not (Test-Path $inFile)) { Fail "No existe la entrada: $inFile"; return }
        Info "Ejecutando con $([IO.Path]::GetFileName($inFile)) ..."
        $r = Start-Proc -File $exe -StdIn $inFile -TimeoutMs ([int]$CFG.timeoutMs) -WorkDir $prob.Dir
        Write-Host $r.StdOut -NoNewline
        if ($r.StdErr.Trim()) { Dim $r.StdErr.TrimEnd() }
        Dim "--- $($r.Ms) ms, exit $($r.ExitCode)$(if ($r.TimedOut) { ', TIMEOUT' })"
    } else {
        Info "Ejecutando (Ctrl+Z + Enter para cerrar la entrada) ..."
        Push-Location $prob.Dir
        try { & $exe } finally { Pop-Location }
    }
}

function Cmd-Test {
    $p = Split-Args $Rest -Switches @('Release', 'San')
    $prob = Resolve-Problem ($p.Pos | Select-Object -First 1)
    $exe = Get-ExePath $prob.Dir $prob.Name

    Info "Compilando $($prob.Name).cpp ..."
    if (-not (Invoke-Build -Source $prob.Source -Exe $exe `
                -Release:([bool](Get-Flag $p 'Release' $false)) `
                -San:([bool](Get-Flag $p 'San' $false)))) { return }

    $testDir = Join-Path $prob.Dir 'tests'
    if (-not (Test-Path $testDir)) { Fail "No hay carpeta tests/ en $($prob.Dir)"; return }

    $only = Get-Flag $p 'Case' ''
    $ins = Get-ChildItem $testDir -Filter '*.in' -File | Sort-Object {
        if ($_.BaseName -match '^\d+$') { [int]$_.BaseName } else { 9999 }
    }, Name
    if ($only) { $ins = $ins | Where-Object { $_.BaseName -eq $only } }
    if (-not $ins) { Warn "No hay casos .in en $testDir"; return }

    $timeout = [int]$CFG.timeoutMs
    $pass = 0; $total = 0
    Write-Host ''

    foreach ($in in $ins) {
        $total++
        if ((Get-Item $in.FullName).Length -eq 0) {
            Warn ("  [{0,-6}] VACIO   - rellena {1}" -f $in.BaseName, $in.Name)
            continue
        }
        $r = Start-Proc -File $exe -StdIn $in.FullName -TimeoutMs $timeout -WorkDir $prob.Dir

        $expFile = Join-Path $testDir "$($in.BaseName).out"
        if (-not (Test-Path $expFile)) { $expFile = Join-Path $testDir "$($in.BaseName).ans" }

        if ($r.TimedOut) {
            Fail ("  [{0,-6}] TLE     > {1} ms" -f $in.BaseName, $timeout)
        } elseif ($r.ExitCode -ne 0) {
            Fail ("  [{0,-6}] RE      exit={1}  ({2} ms)" -f $in.BaseName, $r.ExitCode, $r.Ms)
            if ($r.StdErr.Trim()) { Dim ("           " + ($r.StdErr.Trim() -split "`n")[0]) }
        } elseif (-not (Test-Path $expFile)) {
            Warn ("  [{0,-6}] ?       sin .out esperado  ({1} ms)" -f $in.BaseName, $r.Ms)
            Dim  ("           salida: " + ($r.StdOut.Trim() -replace "`r?`n", ' / '))
        } else {
            $exp = Get-Content $expFile -Raw -Encoding UTF8
            $cmp = Compare-Output -Expected $exp -Actual $r.StdOut
            if ($cmp.Equal) {
                $pass++
                Ok ("  [{0,-6}] OK      {1} ms" -f $in.BaseName, $r.Ms)
            } else {
                Fail ("  [{0,-6}] WA      linea {1}  ({2} ms)" -f $in.BaseName, $cmp.Line, $r.Ms)
                Write-Host ("           esperado : " + $cmp.Expected) -ForegroundColor DarkGreen
                Write-Host ("           obtenido : " + $cmp.Actual)   -ForegroundColor DarkRed
            }
        }
        if ($r.StdErr.Trim() -and $r.ExitCode -eq 0) {
            foreach ($l in ($r.StdErr.TrimEnd() -split "`r?`n")) { Dim "           $l" }
        }
    }

    Write-Host ''
    if ($pass -eq $total) { Ok "$pass/$total casos correctos" }
    else { Warn "$pass/$total casos correctos" }
}

function Cmd-Stress {
    $p = Split-Args $Rest -Switches @('San')
    $prob = Resolve-Problem ($p.Pos | Select-Object -First 1)
    $dir = $prob.Dir

    $brute = Join-Path $dir 'brute.cpp'
    $gen   = Join-Path $dir 'gen.py'
    foreach ($f in @($brute, $gen)) {
        if (-not (Test-Path $f)) {
            $tmpl = Join-Path $ROOT ('templates\' + [IO.Path]::GetFileName($f))
            Copy-Item $tmpl $f
            Warn "Creado $([IO.Path]::GetFileName($f)) desde plantilla - completalo y vuelve a ejecutar."
            return
        }
    }

    $exeMain  = Get-ExePath $dir $prob.Name
    $exeBrute = Get-ExePath $dir 'brute'

    Info 'Compilando main y brute ...'
    $san = [bool](Get-Flag $p 'San' $false)
    if (-not (Invoke-Build -Source $prob.Source -Exe $exeMain -San:$san))  { return }
    if (-not (Invoke-Build -Source $brute -Exe $exeBrute -San:$san)) { return }

    $iters = [int](Get-Flag $p 'Iters' 300)
    $py = (Get-Command python -ErrorAction SilentlyContinue)
    if (-not $py) { Fail 'No se encontro python en el PATH (necesario para gen.py)'; return }

    $tmpIn = Join-Path ([IO.Path]::GetTempPath()) 'cpx_stress.in'
    Info "Stress test: $iters iteraciones"

    for ($i = 1; $i -le $iters; $i++) {
        $g = Start-Proc -File $py.Source -Arguments @($gen, "$i") -TimeoutMs 10000 -WorkDir $dir
        if ($g.ExitCode -ne 0) { Fail "gen.py fallo en la semilla $i"; Write-Host $g.StdErr; return }
        Write-TextFile -Path $tmpIn -Content $g.StdOut

        $a = Start-Proc -File $exeMain  -StdIn $tmpIn -TimeoutMs ([int]$CFG.timeoutMs) -WorkDir $dir
        $b = Start-Proc -File $exeBrute -StdIn $tmpIn -TimeoutMs ([int]$CFG.timeoutMs) -WorkDir $dir

        $bad = $false; $why = ''
        if ($a.TimedOut)          { $bad = $true; $why = 'main dio TLE' }
        elseif ($a.ExitCode -ne 0) { $bad = $true; $why = "main salio con exit=$($a.ExitCode)" }
        elseif (-not (Compare-Output -Expected $b.StdOut -Actual $a.StdOut).Equal) { $bad = $true; $why = 'salidas distintas' }

        if ($bad) {
            Write-Host ''
            Fail "Contraejemplo en la semilla $i - $why"
            Write-Host '--- entrada ---'  -ForegroundColor Yellow
            Write-Host $g.StdOut
            Write-Host '--- brute (esperado) ---' -ForegroundColor DarkGreen
            Write-Host $b.StdOut
            Write-Host '--- main (obtenido) ---'  -ForegroundColor DarkRed
            Write-Host $a.StdOut
            if ($a.StdErr.Trim()) { Dim $a.StdErr.Trim() }

            $testDir = Join-Path $dir 'tests'
            if (-not (Test-Path $testDir)) { New-Item -ItemType Directory $testDir -Force | Out-Null }
            $n = 1
            while (Test-Path (Join-Path $testDir "$n.in")) { $n++ }
            Write-TextFile (Join-Path $testDir "$n.in")  $g.StdOut
            Write-TextFile (Join-Path $testDir "$n.out") $b.StdOut
            Ok "Guardado como tests/$n.in y tests/$n.out"
            return
        }
        if ($i % 10 -eq 0) { Write-Host -NoNewline "`r  ok: $i/$iters" }
    }
    Write-Host ''
    Ok "$iters iteraciones sin diferencias."
}

function Cmd-Tc {
    $p = Split-Args $Rest -Switches @()
    $prob = Resolve-Problem ($p.Pos | Select-Object -First 1)
    $testDir = Join-Path $prob.Dir 'tests'
    if (-not (Test-Path $testDir)) { New-Item -ItemType Directory $testDir -Force | Out-Null }
    $n = 1
    while (Test-Path (Join-Path $testDir "$n.in")) { $n++ }
    foreach ($ext in @('in', 'out')) {
        Write-TextFile (Join-Path $testDir "$n.$ext")
    }
    Ok "Creado tests/$n.in y tests/$n.out"
    Open-InEditor @((Join-Path $testDir "$n.in"), (Join-Path $testDir "$n.out"))
}

function Cmd-Accept {
    $p = Split-Args $Rest -Switches @('NoPush', 'Force')
    $prob = Resolve-Problem ($p.Pos | Select-Object -First 1)
    $dir = $prob.Dir

    $judge = Get-Flag $p 'Judge' $CFG.defaultJudge
    $folder = Split-Path $dir -Leaf
    $parent = Split-Path (Split-Path $dir -Parent) -Leaf

    $defaultId = if ($dir -like "*\contests\*") { "$parent-$folder" } else { $folder }
    $id   = Get-Flag $p 'Id'   $defaultId
    $name = Get-Flag $p 'Name' $folder
    $url  = Get-Flag $p 'Url'  ''
    $tags = @()
    $tagStr = Get-Flag $p 'Tags' ''
    if ($tagStr) { $tags = @($tagStr -split '[,\s]+' | Where-Object { $_ }) }

    $safeId = ($id -replace '[^\w\.-]', '-')
    $dest = Join-Path (Join-Path (Join-Path $ROOT 'solutions') $judge) $safeId
    if ((Test-Path $dest) -and -not (Get-Flag $p 'Force' $false)) {
        Warn "Ya existe $dest - se sobrescribe el codigo (usa -Force para silenciar este aviso)."
    }
    New-Item -ItemType Directory -Path $dest -Force | Out-Null

    Copy-Item $prob.Source (Join-Path $dest 'solution.cpp') -Force
    $srcTests = Join-Path $dir 'tests'
    if (Test-Path $srcTests) {
        $dstTests = Join-Path $dest 'tests'
        New-Item -ItemType Directory -Path $dstTests -Force | Out-Null
        Get-ChildItem $srcTests -File | Where-Object { $_.Length -gt 0 } |
            ForEach-Object { Copy-Item $_.FullName $dstTests -Force }
    }

    $meta = [ordered]@{
        judge = $judge
        id    = $id
        name  = $name
        url   = $url
        tags  = $tags
        date  = (Get-Date -Format 'yyyy-MM-dd')
    }
    Write-TextFile (Join-Path $dest 'meta.json') ($meta | ConvertTo-Json -Depth 4)

    $count = Update-SolutionsIndex
    Ok "Archivado en solutions\$judge\$safeId  (total: $count)"

    # ------------------------------------------------------------------ git
    $relDest = $dest.Substring($ROOT.Length + 1)
    $relSrc  = if ($dir.StartsWith($ROOT)) { $dir.Substring($ROOT.Length + 1) } else { $null }

    $paths = @('solutions')
    if ($relSrc) { $paths += $relSrc }
    Invoke-Git -Arguments (@('add', '--') + $paths) -Quiet | Out-Null

    # exit 0 = no hay nada en el indice; 1 = si lo hay
    $staged = Invoke-Git -Arguments @('diff', '--cached', '--quiet') -Quiet
    if ($staged.ExitCode -eq 0) { Warn 'No hay cambios que commitear (el archivo ya estaba al dia).'; return }

    $msg = Get-Flag $p 'Message' "solve($judge): $id - $name"
    $c = Invoke-Git -Arguments @('commit', '-m', $msg)
    if ($c.ExitCode -ne 0) { Fail 'git commit fallo.'; return }
    Ok "Commit: $msg"

    if (Get-Flag $p 'NoPush' $false) { Dim 'Push omitido (-NoPush).'; return }

    Info 'Subiendo a GitHub ...'
    $push = Invoke-Git -Arguments @('push')
    if ($push.ExitCode -eq 0) { Ok 'Push completado.' }
    else { Fail 'git push fallo - revisa el remoto/credenciales. El commit quedo en local.' }
}

function Cmd-Lib {
    $q = ($Rest | Where-Object { $_ -notmatch '^-' }) -join ' '
    $libDir = Join-Path $ROOT 'library'
    $files = Get-ChildItem $libDir -Recurse -File -Include '*.cpp', '*.hpp'

    if ($q) {
        $files = $files | Where-Object {
            $_.Name -like "*$q*" -or (Select-String -Path $_.FullName -Pattern $q -SimpleMatch -Quiet)
        }
    }
    if (-not $files) { Warn "Sin resultados para '$q'"; return }

    $byCat = $files | Group-Object { Split-Path $_.DirectoryName -Leaf } | Sort-Object Name
    foreach ($g in $byCat) {
        Write-Host ''
        Info "  $($g.Name)/"
        foreach ($f in ($g.Group | Sort-Object Name)) {
            $desc = (Get-Content $f.FullName -TotalCount 3 |
                     Where-Object { $_ -match '^\s*//' } |
                     Select-Object -First 1) -replace '^\s*//\s*', ''
            Write-Host ("    {0,-22} {1}" -f $f.Name, $desc)
        }
    }
    Write-Host ''
    Dim '  Incluir en tu solucion:  #include "graphs/dijkstra.cpp"   (el -I ya lo pone cpx)'
}

function Cmd-Stats {
    $solDir = Join-Path $ROOT 'solutions'
    $metas = Get-ChildItem $solDir -Filter 'meta.json' -Recurse -File -ErrorAction SilentlyContinue
    if (-not $metas) { Warn 'Aun no hay soluciones archivadas.'; return }

    $all = @($metas | ForEach-Object { Get-Content $_.FullName -Raw -Encoding UTF8 | ConvertFrom-Json })
    Write-Host ''
    Ok "Total resueltos: $($all.Count)"
    Write-Host ''
    Info '  Por juez:'
    $all | Group-Object judge | Sort-Object Count -Descending |
        ForEach-Object { Write-Host ("    {0,-16} {1}" -f $_.Name, $_.Count) }

    $tags = $all | ForEach-Object { $_.tags } | Where-Object { $_ }
    if ($tags) {
        Write-Host ''
        Info '  Por tag:'
        $tags | Group-Object | Sort-Object Count -Descending | Select-Object -First 15 |
            ForEach-Object { Write-Host ("    {0,-16} {1}" -f $_.Name, $_.Count) }
    }

    Write-Host ''
    Info '  Ultimos 7 dias:'
    $since = (Get-Date).AddDays(-7).ToString('yyyy-MM-dd')
    $recent = @($all | Where-Object { $_.date -ge $since })
    Write-Host ("    {0} problemas" -f $recent.Count)
    Write-Host ''
}

function Cmd-Sync {
    $p = Split-Args $Rest -Switches @()
    $status = Invoke-Git -Arguments @('status', '--porcelain') -Quiet
    if (-not $status.StdOut.Trim()) { Ok 'Nada que sincronizar, el arbol esta limpio.'; return }

    Dim $status.StdOut.TrimEnd()
    Invoke-Git -Arguments @('add', '-A') -Quiet | Out-Null
    $msg = Get-Flag $p 'Message' ("wip: practica $(Get-Date -Format 'yyyy-MM-dd HH:mm')")
    $c = Invoke-Git -Arguments @('commit', '-m', $msg)
    if ($c.ExitCode -ne 0) { Fail 'git commit fallo.'; return }
    Ok "Commit: $msg"
    $push = Invoke-Git -Arguments @('push')
    if ($push.ExitCode -eq 0) { Ok 'Push completado.' } else { Fail 'git push fallo.' }
}

function Cmd-Doctor {
    Write-Host ''
    Info '  Entorno cpx'
    Write-Host ("    raiz          : {0}" -f $ROOT)

    $gpp = Resolve-Compiler
    if ($gpp) {
        $v = Start-Proc -File $gpp -Arguments @('-dumpversion') -TimeoutMs 15000
        Write-Host ("    g++           : {0} (v{1})" -f $gpp, $v.StdOut.Trim()) -ForegroundColor Green
    } else {
        Write-Host  '    g++           : NO ENCONTRADO' -ForegroundColor Red
        Write-Host  '                    winget install -e --id MSYS2.MSYS2' -ForegroundColor White
        Write-Host  '                    pacman -S mingw-w64-ucrt-x86_64-gcc' -ForegroundColor White
        Write-Host  '                    PATH += C:\msys64\ucrt64\bin' -ForegroundColor White
    }

    foreach ($t in @('python', 'git')) {
        $c = Get-Command $t -ErrorAction SilentlyContinue
        if ($c) { Write-Host ("    {0,-14}: {1}" -f $t, $c.Source) -ForegroundColor Green }
        else    { Write-Host ("    {0,-14}: NO ENCONTRADO" -f $t) -ForegroundColor Red }
    }

    $rem = Invoke-Git -Arguments @('remote', 'get-url', 'origin') -Quiet
    if ($rem.ExitCode -eq 0) { Write-Host ("    remoto git    : {0}" -f $rem.StdOut.Trim()) -ForegroundColor Green }
    else { Write-Host '    remoto git    : sin origin' -ForegroundColor Yellow }

    Write-Host ("    std           : {0}" -f $CFG.std)
    Write-Host ("    timeout       : {0} ms" -f $CFG.timeoutMs)
    Write-Host ''
}

function Cmd-Help {
    Write-Host ''
    Write-Host '  cpx - programacion competitiva en C++' -ForegroundColor Cyan
    Write-Host ''
    Write-Host '  PROBLEMAS' -ForegroundColor White
    Write-Host '    cpx new <nombre> [-Tests n] [-Url u] [-Open]   crea practice/<nombre>/'
    Write-Host '    cpx tc [ruta]                                  anade el siguiente caso de prueba'
    Write-Host ''
    Write-Host '  CONCURSOS' -ForegroundColor White
    Write-Host '    cpx contest <slug> [-Problems A-F] [-Judge cf] [-Url u] [-Brute] [-Open]'
    Write-Host '                                                   crea contests/<fecha>-<slug>/A..F/'
    Write-Host ''
    Write-Host '  EJECUTAR' -ForegroundColor White
    Write-Host '    cpx run   [ruta] [-In tests/1.in] [-Release] [-San]'
    Write-Host '    cpx test  [ruta] [-Case 2] [-Release] [-San]    corre todos los tests/*.in'
    Write-Host '    cpx stress [ruta] [-Iters 300] [-San]           main vs brute con gen.py'
    Write-Host ''
    Write-Host '  ARCHIVO Y GIT' -ForegroundColor White
    Write-Host '    cpx accept [ruta] [-Judge cf] [-Id 1234A] [-Name "..."] [-Url u]'
    Write-Host '               [-Tags "dp,greedy"] [-Message "..."] [-NoPush]'
    Write-Host '                                                   archiva en solutions/ + commit + push'
    Write-Host '    cpx sync  [-Message "..."]                     commitea y sube todo lo pendiente'
    Write-Host ''
    Write-Host '  LIBRERIA' -ForegroundColor White
    Write-Host '    cpx lib [busqueda]                             lista/busca snippets'
    Write-Host '    cpx stats                                      progreso'
    Write-Host '    cpx doctor                                     verifica el entorno'
    Write-Host ''
    Write-Host '  Sin [ruta] se usa la carpeta actual.' -ForegroundColor DarkGray
    Write-Host ''
}

# =========================================================== dispatcher =====
switch -Regex ($Command) {
    '^(new|n)$'          { Cmd-New }
    '^(contest|c)$'      { Cmd-Contest }
    '^(run|r)$'          { Cmd-Run }
    '^(test|t)$'         { Cmd-Test }
    '^(stress|s)$'       { Cmd-Stress }
    '^(tc|addtest)$'     { Cmd-Tc }
    '^(accept|a|ac)$'    { Cmd-Accept }
    '^(lib|l)$'          { Cmd-Lib }
    '^stats$'            { Cmd-Stats }
    '^(sync|push)$'      { Cmd-Sync }
    '^doctor$'           { Cmd-Doctor }
    '^(help|-h|--help)$' { Cmd-Help }
    default {
        Fail "Comando desconocido: $Command"
        Cmd-Help
    }
}
