# Clickgame V2 - ESPLESS

Simple countdown click game for Nucleo L152-RE without ESP32

## Compilation

```bash
mkdir build
cd build

cmake .. -GNinja \
             -DCMAKE_BUILD_TYPE=Develop \
             -DMBED_TARGET=NUCLEO_L152RE \
             -DMBED_UPLOAD_METHOD=STLINK

ninja
```

## Flashing (in build directory)

```bash
openocd \
             -f /usr/share/openocd/scripts/interface/stlink.cfg \
             -f /usr/share/openocd/scripts/target/stm32l1.cfg \
             -c "program clickgame.elf verify reset exit"
```
