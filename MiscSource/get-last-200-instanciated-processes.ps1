# file:     get-last-200-instanciated-processes.ps1 - shows top 200 sysmon PROCESS CREATE events' command-lines, in table view
# exec:     pwsh.exe -file get-last-200-instanciated-processes.ps1
# author:   Ben Mullan 2026

Get-WinEvent -FilterHashtable @{LogName="Microsoft-Windows-Sysmon/Operational"; Id=1} -MaxEvents 200 | % {

    $EventData = ([xml]$_.ToXml()).Event.EventData.Data

    [PSCustomObject]@{
        TimeCreated         = $_.TimeCreated
        Image               = ($EventData | Where-Object {$_.Name -eq "Image"})."#text"
        CommandLine         = ($EventData | Where-Object {$_.Name -eq "CommandLine"})."#text"
        ParentCommandLine   = ($EventData | Where-Object {$_.Name -eq "ParentCommandLine"})."#text"
    }

} | Out-GridView -Wait