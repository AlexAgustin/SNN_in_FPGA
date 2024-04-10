<project xmlns="com.autoesl.autopilot.project" top="simulate_SNN" name="vitis_etapa2" projectType="C/C++">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="includes/defines.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="src/neuron.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="includes/neuron.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="src/snn.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="includes/snn.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="../../src/first_network_tb.cpp" sc="0" tb="1" cflags="-Wno-unknown-pragmas" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

