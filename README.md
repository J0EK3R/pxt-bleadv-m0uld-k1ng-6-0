
> Open this page at [https://j0ek3r.github.io/pxt-bleadv-m0uld-k1ng-6-0/](https://j0ek3r.github.io/pxt-bleadv-m0uld-k1ng-6-0/)

# pxt-bleadv-m0uld-k1ng-6-0

A Microsoft Makecode extension for micro:bit `(V2) only` to control Bluetooth `M0uld K1ng Module 6.0`.

## Use as Extension

This repository can be added as an **extension** in MakeCode.

* open [https://makecode.microbit.org/](https://makecode.microbit.org/)
* click on **New Project**
* click on **Extensions** under the gearwheel menu
* search for **https://github.com/j0ek3r/pxt-bleadv-m0uld-k1ng-6-0** and import

## Basic usage

On start first init your `M0uld K1ng Module 6.0`.
This starts Bluetooth Advertising (sending data one-way) the needed connect telegrams to switch the `M0uld K1ng Module` in Bluetooth mode.

```blocks
MK6.init(MK6.Module.M1)
```

Then - i.E. on pressed button - set a channel's value in percent and advertise data:

```blocks
input.onButtonPressed(Button.A, function () {
    MK6.setChannelAndSend(MK6.Module.M1, MK6.Channel.A, 100)
})
```

Or with consistent changes for multiple channels - first `set` then `send`

```blocks
input.onButtonPressed(Button.A, function () {
    MK6.setChannelAndSend(MK6.Module.M1, MK6.Channel.A, 50)
    MK6.setChannelAndSend(MK6.Module.M1, MK6.Channel.B, 75)
    MK6.sendData(MK6.Module.M1)
})
```

## Advanced usage - finetuning

### Set channel's offset value

This function's parameter `offset_pct` is added to a channel's (non-zero) `set value`.
I.E. if you've plugged-in a motor and the motor needs a certain voltage to start turning use this.

```blocks
MK6.setChannelOffset(MK6.Module.M1, MK6.Channel.A, 25)
```

### Limit channel's maximum value

This function's parameter `maximum_pct` limits the channel's output to a maximum - i.E. 50 %

```blocks
MK6.setChannelMax(MK6.Module.M1, MK6.Channel.A, 50)
```

### Set channel's direction

If this function's boolean parameter `reverse` is set then a channel's output is inverted.

```blocks
MK6.setChannelReverse(MK6.Module.M1, MK6.Channel.A, true)
```

### Set channel's zero-hysteresis

All channel's set values lower than this hystersis value are interpreted as zero (stop)

```blocks
MK6.setZeroHysteresis(MK6.Module.M1, 1)
```
