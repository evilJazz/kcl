
function open(parent, delegate, params, autoOpen, looseItem, immediate)
{
    var component = Qt.createComponent(Qt.resolvedUrl("qrc:/KCL/MessageOverlay.qml"));
    console.log(component.errorString());

    var p;
    if (Qt.isQtObject(delegate))
        p = {"sourceComponent": delegate, "params": params};
    else
        p = {"source": delegate};

    if (typeof(params) != "undefined")
        p.params = params;

    if (typeof(autoOpen) != "undefined")
        p.autoOpen = autoOpen;

    if (typeof(looseItem) != "undefined")
        p.looseItem = looseItem;

    if (typeof(immediate) != "undefined")
        p.fadeEnabled = !immediate;

    var overlay = component.createObject(parent, p);
    return overlay;
}
