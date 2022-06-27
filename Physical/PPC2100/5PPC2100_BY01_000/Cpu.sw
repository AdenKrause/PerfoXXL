<?xml version="1.0" encoding="utf-8"?>
<?AutomationStudio Version=4.1.16.135 SP?>
<SwConfiguration CpuAddress="SL1" xmlns="http://br-automation.co.at/AS/SwConfiguration">
  <TaskClass Name="Cyclic#1">
    <Task Name="flow" Source="Processor.flow.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#2">
    <Task Name="lccount" Source="Processor.lccount.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="seriell" Source="Exposer.Hardware.seriell.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="in_out" Source="Exposer.Hardware.in_out.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="papentf" Source="Exposer.Ablauf.papentf.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="aufnahme" Source="Exposer.Ablauf.aufnahme.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="adjust" Source="Exposer.Ablauf.adjust.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="entladen" Source="Exposer.Ablauf.entladen.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="can_comm" Source="Exposer.Hardware.can_comm.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="expose" Source="Exposer.Ablauf.expose.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="egmio" Source="Processor.egmio.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#3">
    <Task Name="lust_cdd" Source="Exposer.Hardware.lust_cdd.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="ip_srv_c" Source="ip_srv_c.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="motors" Source="Processor.motors.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="preheat" Source="Processor.preheat.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#4">
    <Task Name="pics" Source="Visualisierung.pics.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="sram" Source="sram.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="p_achse" Source="Visualisierung.p_achse.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="motoren" Source="Exposer.Hardware.motoren.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="trolley" Source="Exposer.Ablauf.trolley.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="ip_clt_c" Source="ip_clt_c.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="referenz" Source="Exposer.Ablauf.referenz.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="system" Source="system.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="devtank" Source="Processor.devtank.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="egmauto" Source="Processor.egmauto.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#5">
    <Task Name="file" Source="file.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="xml" Source="xml.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="bluefin" Source="bluefin.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="egmser" Source="Processor.egmser.prg" Memory="UserROM" Language="ANSIC" Debugging="true" Disabled="true" />
    <Task Name="tanks" Source="Processor.tanks.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="hwconfig" Source="hwconfig.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="udp_nl" Source="udp_nl.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#6" />
  <TaskClass Name="Cyclic#7" />
  <TaskClass Name="Cyclic#8" />
  <VcDataObjects>
    <VcDataObject Name="visuhr" Source="Visualisierung.visuhr.dob" Memory="UserROM" Language="Vc" WarningLevel="2" Compress="false" />
  </VcDataObjects>
  <Binaries>
    <BinaryObject Name="asfw" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="ashwd" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="arconfig" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="pvmap" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="sysconf" Source="" Memory="SystemROM" Language="Binary" />
  </Binaries>
  <Libraries>
    <LibraryObject Name="ethernet" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="ethsock" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="fileio" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="visapi" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="asstring" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="standard" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="astime" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="asiomman" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="dataobj" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="runtime" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="dvframe" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="can_lib" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="asarcfg" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="brsystem" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="loopcont" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="asudp" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="sys_lib" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsBrStr" Source="Libraries.AsBrStr.lby" Memory="UserROM" Language="Binary" Debugging="true" />
  </Libraries>
</SwConfiguration>