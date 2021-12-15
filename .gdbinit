define re
    shell make qemu
    target remote localhost:1234
end

file bin/kernel
re