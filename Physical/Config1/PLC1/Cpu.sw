<?xml version="1.0" encoding="utf-8"?>
<?AutomationStudio Version=4.1.16.135 SP?>
<SwConfiguration CpuAddress="SL1" xmlns="http://br-automation.co.at/AS/SwConfiguration">
  <TaskClass Name="Exception" />
  <TaskClass Name="Cyclic#1">
    <Task Name="flow" Source="Processor.flow.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
  </TaskClass>
  <TaskClass Name="Cyclic#2">
    <Task Name="lccount" Source="Processor.lccount.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="seriell" Source="Exposer.Hardware.seriell.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="in_out" Source="Exposer.Hardware.in_out.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="papentf" Source="Exposer.Ablauf.papentf.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="aufnahme" Source="Exposer.Ablauf.aufnahme.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="adjust" Source="Exposer.Ablauf.adjust.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="entladen" Source="Exposer.Ablauf.entladen.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="can_comm" Source="Exposer.Hardware.can_comm.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="expose" Source="Exposer.Ablauf.expose.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="egmio" Source="Processor.egmio.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
  </TaskClass>
  <TaskClass Name="Cyclic#3">
    <Task Name="lust_cdd" Source="Exposer.Hardware.lust_cdd.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="ip_srv_c" Source="ip_srv_c.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="motors" Source="Processor.motors.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="preheat" Source="Processor.preheat.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
  </TaskClass>
  <TaskClass Name="Cyclic#4">
    <Task Name="pics" Source="Visualisierung.pics.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="sram" Source="sram.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="p_achse" Source="Visualisierung.p_achse.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="motoren" Source="Exposer.Hardware.motoren.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="trolley" Source="Exposer.Ablauf.trolley.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="referenz" Source="Exposer.Ablauf.referenz.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="system" Source="system.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="devtank" Source="Processor.devtank.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="egmauto" Source="Processor.egmauto.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="ping" Source="ping.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#5">
    <Task Name="file" Source="file.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="xml" Source="xml.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="bluefin" Source="bluefin.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="egmser" Source="Processor.egmser.prg" Memory="UserROM" Language="ANSIC" Debugging="false" Disabled="true" />
    <Task Name="tanks" Source="Processor.tanks.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="hwconfig" Source="hwconfig.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="udp_nl" Source="udp_nl.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
  </TaskClass>
  <TaskClass Name="Cyclic#6" />
  <TaskClass Name="Cyclic#7" />
  <TaskClass Name="Cyclic#8" />
  <DataObjects>
    <DataObject Name="ioimgcfg" Source="Datenobj.ioimgcfg.dob" Memory="UserROM" Language="Simple" />
    <DataObject Name="can_tab" Source="Datenobj.can_tab.dob" Memory="UserROM" Language="Simple" />
    <DataObject Name="do_imaPP42" Source="Datenobj.do_imaPP420.dob" Memory="UserROM" Language="Simple" />
    <DataObject Name="do_vcpPP42" Source="Datenobj.do_vcpPP420.dob" Memory="UserROM" Language="Simple" />
    <DataObject Name="do_xsoldPP" Source="Datenobj.do_xsoldPP420.dob" Memory="UserROM" Language="Simple" />
  </DataObjects>
  <VcDataObjects>
    <VcDataObject Name="visual" Source="Visualisierung.visual.dob" Memory="UserROM" Language="vc" Version="0.00.0" WarningLevel="2" Compress="false" />
  </VcDataObjects>
  <Binaries>
    <BinaryObject Name="iomap2" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="iomap1" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="arcfg2" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="arcfg1" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="AsHW" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="sysconf" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="vcbmpng" Source="" Memory="UserROM" Language="Binary" />
  </Binaries>
  <Libraries>
    <LibraryObject Name="visapi" Source="Libraries.visapi.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="sys_lib" Source="Libraries.sys_lib.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="standard" Source="Libraries.standard.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="runtime" Source="Libraries.runtime.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="OPERATOR" Source="Libraries.OPERATOR.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="LoopCont" Source="Libraries.LoopCont.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="INACLNT" Source="Libraries.INACLNT.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="FileIO" Source="Libraries.FileIO.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="EthSock" Source="Libraries.EthSock.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="Ethernet" Source="Libraries.Ethernet.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="dvframe" Source="Libraries.dvframe.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="DataObj" Source="Libraries.DataObj.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="CONVERT" Source="Libraries.CONVERT.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="CAN_Lib" Source="Libraries.CAN_Lib.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="brsystem" Source="Libraries.brsystem.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsUDP" Source="Libraries.AsUDP.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsTime" Source="Libraries.AsTime.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsString" Source="Libraries.AsString.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsIOMMan" Source="Libraries.AsIOMMan.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsIMA" Source="Libraries.AsIMA.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsARCfg" Source="Libraries.AsARCfg.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsBrStr" Source="Libraries.AsBrStr.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsICMP" Source="Libraries.AsICMP.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsUSB" Source="Libraries.AsUSB.lby" Memory="UserROM" Language="Binary" Debugging="true" />
  </Libraries>
</SwConfiguration>