## ESPHome LCTech Multi-Channel Relay Module

ESPHome custom module and example configuration to drive [LCTech Inc.](http://www.lctech-inc.com/) multi-channel relay modules.
Tested on a late-2020 12V, 4-channel version with a nuvoton MCU.


## Running & flashing

```
cp 4ch-relay.yaml.dist my-4ch-relay.yaml
$EDITOR my-4ch-relay.yaml  # edit where it says "XXX_CHANGEME_XXX"
esphome my-4ch-relay.yaml run
```


## Legal stuff

Copyright © 2021 Kai Fabian -- MIT license.
See [LICENSE.md](LICENSE.md) for the specific licensing conditions.

Loosely based on [`esphome-lctech-4chanel-modules`](https://github.com/nekromant/esphome-lctech-4chanel-modules) by Andres 'Necromant' Andrianov.
