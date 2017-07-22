'@echo off
setlocal disableDelayedExpansion
for /f "delims=" %%A in ('forfiles /p ..\STM32F1\binaries /s /m generic*.bin /c "cmd /c echo @relpath"') do (
  set "file=%%~A"
  setlocal enableDelayedExpansion
  .\bin_merge_tool\mergesketch.exe ..\STM32F1\binaries\!file:~2! .\congratulations_sketch\congratulations_sketch.ino.generic_stm32f103c.bin .\merged_bins\!file:~2!
  endlocal
)

.\bin_merge_tool\mergesketch.exe ..\STM32F1\binaries\naze32_boot20.bin .\congratulations_sketch\congratulations_sketch.ino.generic_stm32f103c.bin .\merged_bins\naze32_boot20.bin
.\bin_merge_tool\mergesketch.exe ..\STM32F1\binaries\stbee_boot20.bin  .\congratulations_sketch\congratulations_sketch.ino.generic_stm32f103c.bin .\merged_bins\stbee_boot20.bin

setlocal disableDelayedExpansion
for /f "delims=" %%A in ('forfiles /p ..\STM32F1\binaries /s /m maple*.bin /c "cmd /c echo @relpath"') do (
  set "file=%%~A"
  setlocal enableDelayedExpansion
  .\bin_merge_tool\mergesketch.exe ..\STM32F1\binaries\!file:~2! .\congratulations_sketch\congratulations_sketch.ino.maple_mini.bin .\merged_bins\!file:~2!
  endlocal
)
