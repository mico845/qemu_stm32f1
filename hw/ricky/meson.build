ricky_ss = ss.source_set()

ricky_ss.add(files('../arm/boot.c'))

ricky_ss.add(files('../arm/armv7m.c'))
ricky_ss.add(files('../arm/exynos4210.c'))
ricky_ss.add(files('../arm/digic.c'))
ricky_ss.add(files('../arm/omap1.c'))


ricky_ss.add(when: 'CONFIG_RICKY_BOARD', if_true: files(
                        'ricky_board.c',
                  ))

hw_arch += {'arm': ricky_ss}
