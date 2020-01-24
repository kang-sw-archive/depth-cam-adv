Option explicit
Dim oShell
set oShell= Wscript.CreateObject("WScript.Shell")
oShell.Run "cmd"
oShell.Sendkeys "telnet"
oShell.Sendkeys "{enter}"
oShell.Sleep 1000
oShell.Sendkeys "open localhost 4444"
oShell.Sleep 1000
oShell.Sendkeys "reset halt"
oShell.Sleep 1000
oShell.Sendkeys "reset init"
oShell.Sleep 1000
oShell.Sendkeys "program E:\__LocalWorkspace\Edu-Projects\depth-cam-adv\projects\depscan-firmware-rtos\build\depscan-firmware-rtos.elf"
oShell.Sleep 1000
Wscript.Quit
