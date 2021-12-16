set remotetimeout 120 
set tcp auto-retry on
#handle all nostop
#handle all pass
#handle SIGINT nostop
#handle SIGINT pass
#handle SIGTRAP nostop
#handle SIGTRAP pass
target remote localhost:1234

define re
    shell make qemu
    target remote localhost:1234
end

file bin/kernel
re

define hook-quit
    set confirm off
end

define astep
    stepi
    disas
end

define astep2
    stepi
    x/10i $eip
end

define b_exception
    b exception0
    b exception1
    b exception2
    b exception3
    b exception4
    b exception5
    b exception6
    b exception7
    b exception8
    b exception9
    b exception10
    b exception11
    b exception12
    b exception13
    b exception14
    b exception15
    b exception16
    b exception17
    b exception18
    b exception19
    b exception20
    b exception21
    b exception22
    b exception23
    b exception24
    b exception25
    b exception26
    b exception27
    b exception28
    b exception29
    b exception30
    b exception31
    b exception128
end

define xxd
    dump binary memory dump.bin $arg0 $arg0+$arg1
    shell xxd dump.bin
end