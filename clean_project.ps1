
# 获取当前脚本所在的目录，作为项目的根目录
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition

# 切换到项目根目录
Set-Location $projectRoot

# 定义要清理的文件和目录列表，基于 .gitignore 中的条目
# 注意：这里不会列出要保留的文件或目录，排除逻辑在后面处理
$cleanupPatterns = @(
    "*.map",
    # Removed: "*.syscfg", # 不再清理所有的.syscfg文件
    "*.uvguix.*",
    "*.uvoptx",
    "JLinkLog.txt",
    "JLinkSettings.ini",
    "*.lst",
    "syscfg_c.rov.xs",
    "ElectroRace2025/project/Objects/", # 清理整个 Objects directory
    "ElectroRace2025/kernel/freertos/makefile",
    "ElectroRace2025/kernel/freertos/rov/",
    "ElectroRace2025/kernel/freertos/rov_theia/",
    "ElectroRace2025/kernel/freertos/Source/portable/*/*.o",
    "ElectroRace2025/kernel/freertos/Source/portable/*/*.d",
    "ElectroRace2025/kernel/freertos/Source/portable/*/*/*.o",
    "ElectroRace2025/kernel/freertos/Source/portable/*/*/*.d",
    "ElectroRace2025/kernel/freertos/dpl/*.o",
    "ElectroRace2025/kernel/freertos/dpl/*.d",
    "ElectroRace2025/kernel/freertos/Source/*.o",
    "ElectroRace2025/kernel/freertos/Source/*.d",
    "ElectroRace2025/source/third_party/u8g2/csrc/*.o",
    "ElectroRace2025/source/third_party/u8g2/csrc/*.d",
    "ElectroRace2025/source/ti/driverlib/.meta/",
    "ElectroRace2025/source/ti/driverlib/lib/*/",
    "ElectroRace2025/source/ti/driverlib/lib/*/*/",
    "ElectroRace2025/source/ti/driverlib/*/*.o",
    "ElectroRace2025/source/ti/driverlib/*/*.d",
    "ElectroRace2025/source/ti/driverlib/*/*/*.o",
    "ElectroRace2025/source/ti/driverlib/*/*/*.d",
    "ElectroRace2025/application/**/*.o",
    "ElectroRace2025/application/**/*.d",
    "ElectroRace2025/tools/*.cfg", # 清理工具目录下的特定临时文件
    "ElectroRace2025/tools/*.bat",  # 清理工具目录下的特定临时文件
    "*.kjmsd", # 添加清理以.kjmsd结尾的文件
    ".metadata/"
)

# 需要保留的 FreeRTOS builds 目录的完整路径前缀 (用于排除)
$keptFreeRTOSBuildsDirPathPrefix = Join-Path -Path $projectRoot -ChildPath "ElectroRace2025/kernel/freertos/builds/"

# 需要保留的 tools 目录的完整路径前缀 (用于排除)
$keptToolsDirPathPrefix = Join-Path -Path $projectRoot -ChildPath "ElectroRace2025/tools/"

# 需要保留的 DriverLib 库文件目录的完整路径前缀 (用于排除)
$keptDriverLibKeilLibDirPathPrefix = Join-Path -Path $projectRoot -ChildPath "ElectroRace2025/source/ti/driverlib/lib/keil/m0p/mspm0g1x0x_g3x0x/"

Write-Host "开始清理项目中的临时文件..."

foreach ($pattern in $cleanupPatterns) {
    Write-Host "查找匹配模式: $pattern"
    # 使用 Get-ChildItem 查找匹配模式的文件和目录
    $itemsToClean = @(
        try {
            Get-ChildItem -Path $projectRoot -Filter $pattern -Recurse -ErrorAction SilentlyContinue
        } catch {
            # 忽略找不到路径或匹配项的错误
        }
    )

    if ($itemsToClean.Count -gt 0) {
         Write-Host "找到 $($itemsToClean.Count) 个匹配项进行清理..."
         $itemsToClean | ForEach-Object {
             $itemFullName = $_.FullName
             $isContainer = $_.PSIsContainer

             # 检查是否是需要保留的 FreeRTOS builds 目录或其内容
             if ($itemFullName.StartsWith($keptFreeRTOSBuildsDirPathPrefix)) {
                 Write-Host "跳过保留的 FreeRTOS builds 目录或其内容: $itemFullName"
                 return # 跳过当前项目
             }

             # 检查是否是需要保留的 tools 目录或其内容
             if ($itemFullName.StartsWith($keptToolsDirPathPrefix)) {
                  Write-Host "跳过保留的 tools 目录或其内容: $itemFullName"
                  return # 跳过当前项目
             }

             # 检查是否是需要保留的 DriverLib 库文件目录或其内容
             if ($itemFullName.StartsWith($keptDriverLibKeilLibDirPathPrefix)) {
                  Write-Host "跳过保留的 DriverLib 库文件目录或其内容: $itemFullName"
                  return # 跳过当前项目
             }

             # 检查是否是需要保留的 .sct 文件
             if (-not $isContainer -and ($_.Extension -eq ".sct")) {
                 Write-Host "跳过保留的 .sct 文件: $itemFullName"
                 return # 跳过当前项目
             }

             # 检查是否是需要保留的 .syscfg 文件
             if (-not $isContainer -and ($_.Extension -eq ".syscfg")) {
                  Write-Host "跳过保留的 .syscfg 文件: $itemFullName"
                  return # 跳过当前项目
             }

             # 检查是否是需要保留的 Keil 项目文件
             if (-not $isContainer -and $_.Name -eq "freertos_demo.uvprojx" -and (Split-Path -Parent $itemFullName) -eq $projectRoot) {
                 Write-Host "跳过保留的 Keil 项目文件: $itemFullName"
                 return # 跳过当前项目
             }

             # 如果通过所有排除检查，则进行删除
             Write-Host "正在清理: $itemFullName"
             Remove-Item -Path $itemFullName -Recurse:$isContainer -Force -ErrorAction SilentlyContinue
         }
    }
}

Write-Host "项目清理完成！"
