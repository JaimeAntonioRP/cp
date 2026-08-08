# ---------------------------------------------------------------------------
#  common.ps1 - utilidades compartidas por todos los comandos de cpx
# ---------------------------------------------------------------------------

function Get-CpRoot {
    return (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}

function Get-CpConfig {
    $path = Join-Path (Get-CpRoot) 'config.json'
    if (-not (Test-Path $path)) { throw "No se encontro config.json en $(Get-CpRoot)" }
    return (Get-Content $path -Raw -Encoding UTF8 | ConvertFrom-Json)
}

# ---------------------------------------------------------------- escritura
# Set-Content -Encoding UTF8 anade BOM en PowerShell 5.1: un .in "vacio" pesaria
# 3 bytes y el BOM acabaria en el stdin del programa. Siempre escribir con esto.
function Write-TextFile {
    param([Parameter(Mandatory)][string]$Path, [string]$Content = '')
    $dir = Split-Path $Path -Parent
    if ($dir -and -not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
    [IO.File]::WriteAllText($Path, $Content, (New-Object Text.UTF8Encoding($false)))
}

# --------------------------------------------------------------------- salida
function Info($m) { Write-Host $m -ForegroundColor Cyan }
function Ok($m)   { Write-Host $m -ForegroundColor Green }
function Warn($m) { Write-Host $m -ForegroundColor Yellow }
function Fail($m) { Write-Host $m -ForegroundColor Red }
function Dim($m)  { Write-Host $m -ForegroundColor DarkGray }

# ------------------------------------------------------------------ compilador
function Resolve-Compiler {
    $cfg = Get-CpConfig
    $cc = Get-Command $cfg.compiler -ErrorAction SilentlyContinue
    if ($cc) { return $cc.Source }
    $candidates = @(
        'C:\msys64\ucrt64\bin\g++.exe',
        'C:\msys64\mingw64\bin\g++.exe',
        'C:\mingw64\bin\g++.exe',
        'C:\MinGW\bin\g++.exe'
    )
    foreach ($p in $candidates) { if (Test-Path $p) { return $p } }

    # Instalado por winget (WinLibs) pero el PATH de esta consola aun no se refresco
    $wg = Join-Path $env:LOCALAPPDATA 'Microsoft\WinGet\Packages'
    if (Test-Path $wg) {
        $hit = Get-ChildItem $wg -Filter 'g++.exe' -Recurse -File -ErrorAction SilentlyContinue |
               Select-Object -First 1
        if ($hit) { return $hit.FullName }
    }
    return $null
}

function Assert-Compiler {
    $gpp = Resolve-Compiler
    if (-not $gpp) {
        Fail "No se encontro g++ en el PATH."
        Warn "Instala MinGW-w64 con:"
        Write-Host "    winget install -e --id MSYS2.MSYS2" -ForegroundColor White
        Write-Host "  y luego en la terminal MSYS2:  pacman -S mingw-w64-ucrt-x86_64-gcc" -ForegroundColor White
        Write-Host "  Anade C:\msys64\ucrt64\bin al PATH.  Verifica con: cpx doctor" -ForegroundColor White
        throw 'compilador ausente'
    }
    return $gpp
}

# ---------------------------------------------------------------- subprocesos
function ConvertTo-QuotedArg([string]$a) {
    if ($a -match '[\s"]') { return '"' + ($a -replace '"', '\"') + '"' }
    return $a
}

<#
  Ejecuta un programa capturando stdout/stderr en ficheros (sin riesgo de
  deadlock) y midiendo el tiempo de pared.  Devuelve un objeto con
  ExitCode / StdOut / StdErr / Ms / TimedOut.
#>
function Start-Proc {
    param(
        [Parameter(Mandatory)][string]$File,
        [string[]]$Arguments = @(),
        [string]$StdIn,
        [int]$TimeoutMs = 0,
        [string]$WorkDir
    )
    $outFile = [IO.Path]::GetTempFileName()
    $errFile = [IO.Path]::GetTempFileName()

    $sp = @{
        FilePath               = $File
        NoNewWindow            = $true
        PassThru               = $true
        RedirectStandardOutput = $outFile
        RedirectStandardError  = $errFile
    }
    if ($Arguments.Count -gt 0) {
        $sp.ArgumentList = (($Arguments | ForEach-Object { ConvertTo-QuotedArg $_ }) -join ' ')
    }
    if ($WorkDir) { $sp.WorkingDirectory = $WorkDir }
    if ($StdIn -and (Test-Path $StdIn)) { $sp.RedirectStandardInput = $StdIn }

    $sw = [Diagnostics.Stopwatch]::StartNew()
    $p = Start-Process @sp
    # Tocar .Handle cachea el handle del proceso; sin esto .ExitCode queda vacio
    # al usar -PassThru sin -Wait (comportamiento documentado de PowerShell 5.1).
    try { $null = $p.Handle } catch { }
    $timedOut = $false
    if ($TimeoutMs -gt 0) {
        if (-not $p.WaitForExit($TimeoutMs)) {
            $timedOut = $true
            try { $p.Kill() } catch { }
        }
    }
    $p.WaitForExit()
    $sw.Stop()

    $o = ''; $e = ''
    if (Test-Path $outFile) { $o = [IO.File]::ReadAllText($outFile) }
    if (Test-Path $errFile) { $e = [IO.File]::ReadAllText($errFile) }
    Remove-Item $outFile, $errFile -Force -ErrorAction SilentlyContinue

    return [pscustomobject]@{
        ExitCode = $p.ExitCode
        StdOut   = $o
        StdErr   = $e
        Ms       = [int]$sw.Elapsed.TotalMilliseconds
        TimedOut = $timedOut
    }
}

# ------------------------------------------------------------------ compilar
function Invoke-Build {
    param(
        [Parameter(Mandatory)][string]$Source,
        [Parameter(Mandatory)][string]$Exe,
        [switch]$Release,
        [switch]$San
    )
    $gpp  = Assert-Compiler
    $cfg  = Get-CpConfig
    $root = Get-CpRoot

    $flags = @("-std=$($cfg.std)")
    if ($Release) { $flags += $cfg.releaseFlags } else { $flags += $cfg.debugFlags }
    if ($San)     { $flags += $cfg.sanitizeFlags }
    $flags += @('-I', (Join-Path $root 'library'), '-I', (Join-Path $root 'library\misc'))

    $binDir = Split-Path $Exe -Parent
    if (-not (Test-Path $binDir)) { New-Item -ItemType Directory -Path $binDir -Force | Out-Null }

    $args = @($Source) + $flags + @('-o', $Exe)
    Dim ("  g++ " + (($args | ForEach-Object { Split-Path $_ -Leaf }) -join ' '))

    $r = Start-Proc -File $gpp -Arguments $args -TimeoutMs 60000

    # MinGW-w64 no distribuye libasan/libubsan. Si el enlazado falla por eso,
    # reintentamos con UBSan en modo trap, que no necesita runtime.
    if ($San -and $r.ExitCode -ne 0 -and $r.StdErr -match '-l(asan|ubsan|tsan)') {
        Warn '  Esta instalacion de g++ no trae libasan/libubsan; uso UBSan en modo trap.'
        Dim  '  (la UB abortara el programa con "Illegal instruction", sin mensaje detallado)'
        $flags = @($flags | Where-Object { $_ -notin $cfg.sanitizeFlags }) + $cfg.sanitizeFallbackFlags
        $args = @($Source) + $flags + @('-o', $Exe)
        $r = Start-Proc -File $gpp -Arguments $args -TimeoutMs 60000
    }

    if ($r.ExitCode -ne 0) {
        Fail "--- error de compilacion ---"
        Write-Host $r.StdErr
        return $false
    }
    if ($r.StdErr.Trim()) {
        Warn "--- warnings ---"
        Write-Host $r.StdErr
    }
    return $true
}

# ------------------------------------------------------- comparacion de salida
# Compara ignorando espacios en blanco al final de linea y lineas vacias finales.
function Compare-Output {
    param([string]$Expected, [string]$Actual)

    $normalize = {
        param($t)
        if ([string]::IsNullOrEmpty($t)) { return }
        # El @() es imprescindible: con una sola linea, PowerShell colapsa el
        # resultado a una cadena y luego $e[0] indexaria CARACTERES en vez de
        # lineas ("YES" -> "Y"), dando un WA fantasma en ficheros sin salto final.
        $lines = @($t -replace "`r`n", "`n" -split "`n" | ForEach-Object { $_.TrimEnd() })
        while ($lines.Count -gt 0 -and $lines[-1] -eq '') {
            $lines = @($lines[0..($lines.Count - 2)])
        }
        return $lines
    }

    $e = @(& $normalize $Expected)
    $a = @(& $normalize $Actual)

    $n = [Math]::Max($e.Count, $a.Count)
    for ($i = 0; $i -lt $n; $i++) {
        $le = if ($i -lt $e.Count) { $e[$i] } else { '<sin linea>' }
        $la = if ($i -lt $a.Count) { $a[$i] } else { '<sin linea>' }
        if ($le -ne $la) {
            return [pscustomobject]@{ Equal = $false; Line = $i + 1; Expected = $le; Actual = $la }
        }
    }
    return [pscustomobject]@{ Equal = $true; Line = 0; Expected = ''; Actual = '' }
}

# ------------------------------------------------------- resolucion de rutas
<#
  Acepta: nada (cwd), una carpeta de problema, o un .cpp.
  Devuelve un objeto con Dir, Source, Name.
#>
function Resolve-Problem {
    param([string]$Path)

    if (-not $Path) { $Path = (Get-Location).Path }
    if (-not (Test-Path $Path)) {
        # segundo intento: relativa a la raiz del repo (para llamar desde cualquier sitio)
        $alt = Join-Path (Get-CpRoot) $Path
        if (Test-Path $alt) { $Path = $alt }
        else { throw "No existe la ruta: $Path" }
    }

    $item = Get-Item $Path
    if ($item.PSIsContainer) {
        $dir = $item.FullName
        $src = Join-Path $dir 'main.cpp'
        if (-not (Test-Path $src)) {
            $cand = Get-ChildItem $dir -Filter '*.cpp' -File |
                    Where-Object { $_.Name -ne 'brute.cpp' } |
                    Select-Object -First 1
            if (-not $cand) { throw "No hay ningun .cpp en $dir" }
            $src = $cand.FullName
        }
    } else {
        $dir = $item.DirectoryName
        $src = $item.FullName
    }

    return [pscustomobject]@{
        Dir    = $dir
        Source = $src
        Name   = [IO.Path]::GetFileNameWithoutExtension($src)
    }
}

function Get-ExePath([string]$Dir, [string]$Name) {
    return (Join-Path (Join-Path $Dir 'bin') "$Name.exe")
}

# ------------------------------------------------------------------- git
function Invoke-Git {
    param([string[]]$Arguments, [switch]$Quiet)
    $root = Get-CpRoot
    $r = Start-Proc -File 'git' -Arguments (@('-C', $root) + $Arguments) -TimeoutMs 120000
    if (-not $Quiet) {
        if ($r.StdOut.Trim()) { Dim $r.StdOut.Trim() }
        if ($r.StdErr.Trim()) { Dim $r.StdErr.Trim() }
    }
    return $r
}

# -------------------------------------------------- indice de soluciones
function Update-SolutionsIndex {
    $root = Get-CpRoot
    $solDir = Join-Path $root 'solutions'
    if (-not (Test-Path $solDir)) { return }

    $metas = Get-ChildItem $solDir -Filter 'meta.json' -Recurse -File -ErrorAction SilentlyContinue
    $rows = @()
    foreach ($m in $metas) {
        try { $j = Get-Content $m.FullName -Raw -Encoding UTF8 | ConvertFrom-Json } catch { continue }
        # ruta relativa AL PROPIO README (que vive dentro de solutions/)
        $rel = $m.DirectoryName.Substring($solDir.Length + 1).Replace('\', '/')
        $name = if ($j.name) { $j.name } else { $j.id }
        $link = if ($j.url) { "[$name]($($j.url))" } else { $name }
        $tags = if ($j.tags) { ($j.tags -join ', ') } else { '' }
        $rows += [pscustomobject]@{
            Date  = $j.date
            Judge = $j.judge
            Id    = $j.id
            Cell  = "| $($j.date) | $($j.judge) | ``$($j.id)`` | $link | $tags | [codigo]($rel) |"
        }
    }

    $sorted = $rows | Sort-Object -Property Date -Descending
    $lines = @()
    $lines += '# Soluciones aceptadas'
    $lines += ''
    $lines += "Total: **$($rows.Count)** problemas.  Generado por ``cpx accept`` - no editar a mano."
    $lines += ''
    $lines += '| Fecha | Juez | ID | Problema | Tags | Codigo |'
    $lines += '| --- | --- | --- | --- | --- | --- |'
    foreach ($r in $sorted) { $lines += $r.Cell }
    $lines += ''

    Write-TextFile -Path (Join-Path $solDir 'README.md') -Content (($lines -join "`r`n") + "`r`n")
    return $rows.Count
}
