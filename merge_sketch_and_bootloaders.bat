'@echo off
setlocal disableDelayedExpansion
for /f "delims=" %%A in ('forfiles /p .\bootloader_only_binaries /s /m *.bin /c "cmd /c echo @relpath"') do (
  set "file=%%~A"
  setlocal enableDelayedExpansion
  .\sketch_combiner\bin_merge_tool\mergesketch.exe .\bootloader_only_binaries\!file:~2! .\sketch_combiner\congratulations_sketch\congratulations_sketch.ino.generic_stm32f103c.bin .\binaries\!file:~2!
  endlocal
)



setlocal disableDelayedExpansion
for /f "delims=" %%A in ('forfiles /p .\bootloader_only_binaries /s /m maple*.bin /c "cmd /c echo @relpath"') do (
  set "file=%%~A"
  setlocal enableDelayedExpansion
  .\sketch_combiner\bin_merge_tool\mergesketch.exe .\bootloader_only_binaries\!file:~2! .\sketch_combiner\congratulations_sketch\congratulations_sketch.ino.maple_mini.bin .\binaries\!file:~2!
  endlocal
)

.\sketch_combiner\bin_merge_tool\mergesketch.exe .\bootloader_only_binaries\gd32f1_generic_boot20_pc13.bin  .\sketch_combiner\congratulations_sketch\congratulations_sketch.ino.generic_gd32f103c.bin .\binaries\gd32f1_generic_boot20_pc13.bin
.\sketch_combiner\bin_merge_tool\mergesketch.exe .\bootloader_only_binaries\gd32f1_frankenmaple.bin  .\sketch_combiner\congratulations_sketch\congratulations_sketch.ino.generic_gd32f103c.bin .\binaries\gd32f1_frankenmaple.bin

