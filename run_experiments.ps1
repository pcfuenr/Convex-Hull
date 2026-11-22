param(
    [int]$Runs = 32
)

$ErrorActionPreference = "Stop"
$Algos = @("jarvis", "dnc", "graham")
$OutDir = "resultados"

Write-Host "Compilando uhr.cpp..." -ForegroundColor Cyan
g++ -std=c++11 -O0 uhr.cpp -o uhr.exe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$Files = @(Get-ChildItem -Path "datasets" -File -ErrorAction SilentlyContinue)
$NumDatasets = $Files.Count

if ($NumDatasets -eq 0) {
    Write-Error "No hay archivos en la carpeta datasets/."
    exit 1
}

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

for ($ds = 1; $ds -le $NumDatasets; $ds++) {
    foreach ($algo in $Algos) {
        Write-Host ">>> Ejecutando $algo en dataset $ds ($Runs corridas)..." -ForegroundColor Yellow
        .\uhr.exe "$OutDir\${algo}_dataset${ds}.csv" $Runs $algo $ds
    }
}

Write-Host "Todos los experimentos finalizaron. Resultados en $OutDir\" -ForegroundColor Green
