# file:    Run-CSharpInMemory.ps1 - executes a c# file (inc a class + Main(string[] args)) entirely in-memory (no exe on disk)
# exec:    powershell -File .\Run-CSharpInMemory.ps1 -SourceFile .\Run-CSharpInMemory-Demo.cs -Args "foo","bar"
# NOTE:    USE WINDOWS `powershell`; NOT `pwsh` CORE
# author:  Copilot + Ben Mullan (c) 2025

param(
    [Parameter(Mandatory=$true)]
    [string]$SourceFile,

    [string[]]$Args
)

if (-not (Test-Path $SourceFile)) {
    Write-Error "File not found: $SourceFile"
    exit 1
}

$rawSource = Get-Content $SourceFile -Raw

# Use CodeDom to compile into a new in-memory assembly each run
$provider = New-Object Microsoft.CSharp.CSharpCodeProvider
$params = New-Object System.CodeDom.Compiler.CompilerParameters
$params.GenerateInMemory = $true
$params.GenerateExecutable = $false

$results = $provider.CompileAssemblyFromSource($params, $rawSource)

if ($results.Errors.HasErrors) {
    $results.Errors | ForEach-Object { Write-Error $_.ToString() }
    exit 1
}

$asm = $results.CompiledAssembly
$type = $asm.GetTypes() | Where-Object { $_.GetMethod("Main", [Type[]]@([string[]])) } | Select-Object -First 1

if (-not $type) {
    Write-Error "No type with a static Main(string[] args) found."
    exit 1
}

$method = $type.GetMethod("Main", [Reflection.BindingFlags] "Public,Static", $null, [Type[]]@([string[]]), $null)

if (-not $method) {
    Write-Error "Could not locate Main(string[] args)."
    exit 1
}

# Invoke Main with arguments
$splitArgs = $Args -split ','
$method.Invoke($null, (, $splitArgs))