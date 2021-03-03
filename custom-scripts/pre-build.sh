#!/bin/bash

cp $BASE_DIR/../custom-scripts/S41network-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S41network-config

cp $BASE_DIR/../custom-scripts/S50load-drivers $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S50load-drivers

cp $BASE_DIR/../custom-scripts/S52autotune $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S52autotune

tar -zxvf $BASE_DIR/../custom-scripts/packages/xenomai-3.1.tar.gz -C $BASE_DIR/build
pushd $BASE_DIR/build/xenomai-3.1
scripts/bootstrap
export CFLAGS="-march=armv7-a -mfloat-abi=hard -mfpu=neon -ffast-math"
export LDFLAGS="-march=armv7-a -mfloat-abi=hard -mfpu=neon -ffast-math"
export DESTDIR=$BASE_DIR/target
mkdir -p build
pushd build
../configure --enable-smp --host=arm-linux-gnueabihf --with-core=cobalt
make && make install
popd
popd

make -C $BASE_DIR/../custom-scripts/gpio_test/
cp  $BASE_DIR/../custom-scripts/gpio_test/xen-gpio $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/spi_test
cp  $BASE_DIR/../custom-scripts/spi_test/xen_spi $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/hello_task
cp  $BASE_DIR/../custom-scripts/hello_task/hello_task $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/multi_task
cp  $BASE_DIR/../custom-scripts/multi_task/multi_task $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/multi_task/multi_task2 $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/semaphore
cp  $BASE_DIR/../custom-scripts/semaphore/sem_no_sync $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/semaphore/sem_toggle $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/scheduling
cp  $BASE_DIR/../custom-scripts/scheduling/priority_sched $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/scheduling/priority_sched2 $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/scheduling/priority_sched3 $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/scheduling/priority_sched4 $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/rrb_scheduling
cp  $BASE_DIR/../custom-scripts/rrb_scheduling/rrb_sched $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/rrb_scheduling/rrb_sched2 $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/rrb_scheduling/rrb_sched3 $BASE_DIR/target/usr/xenomai/bin

make -C $BASE_DIR/../custom-scripts/gpio_led
cp  $BASE_DIR/../custom-scripts/gpio_led/gpio_led $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/gpio_led/gpio_led2 $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/gpio_led/gpio_led_button $BASE_DIR/target/usr/xenomai/bin
c

make -C $BASE_DIR/../custom-scripts/jitter_and_latency
cp  $BASE_DIR/../custom-scripts/jitter_and_latency/jitter_and_latency $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/jitter_and_latency/jitter_and_latency2 $BASE_DIR/target/usr/xenomai/bin
cp  $BASE_DIR/../custom-scripts/jitter_and_latency/jitter_and_latency_no_worker $BASE_DIR/target/usr/xenomai/bin


#FIXME: This entry will repeat at each recompilation.
echo -e >> $BASE_DIR/target/etc/profile
echo -e "export LD_LIBRARY_PATH=/usr/xenomai/lib" >> $BASE_DIR/target/etc/profile




