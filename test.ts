// tests go here; this will not be compiled when this package is used as an extension.
input.onButtonPressed(Button.A, function () {
    MK6.setChannelAndSend(MK6.Module.M1, MK6.Channel.A, 50)
})
input.onButtonPressed(Button.B, function () {
    MK6.setChannelAndSend(MK6.Module.M1, MK6.Channel.A, 0)
})
MK6.init(MK6.Module.M1)
MK6.setChannelOffset(MK6.Module.M1, MK6.Channel.A, 20)
