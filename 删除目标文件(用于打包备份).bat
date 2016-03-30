rd /Q /S project\MDK-ARM(uV4)\Flash
rd /Q /S project\MDK-ARM(uV4)\CpuRAM
rd /Q /S project\MDK-ARM(uV4)\ExtSRAM
del /Q project\MDK-ARM(uV4)\*.bak
del /Q project\MDK-ARM(uV4)\*.dep
del /Q project\MDK-ARM(uV4)\JLink*
del /Q project\MDK-ARM(uV4)\project.uvgui.*

del /Q project\EWARMv6\Project.dep
del /Q project\EWARMv6\Flash
del /Q project\EWARMv6\CpuRAM
del /Q project\EWARMv6\settings
rd  /Q /S project\EWARMv6\Flash
rd /Q /S project\EWARMv6\CpuRAM
rd /Q /S project\EWARMv6\settings

