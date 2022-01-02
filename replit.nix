{ pkgs }: {
    deps = [
      pkgs.gdb-multitarget
      pkgs.valgrind
      pkgs.python310
			pkgs.gcc
    ];
}