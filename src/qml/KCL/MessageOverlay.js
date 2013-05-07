
function open(parent, delegateUrl, params)
{
    var component = Qt.createComponent(Qt.resolvedUrl("qrc:/KCL/MessageOverlay.qml"));
    console.log(component.errorString());
    var overlay = component.createObject(parent, {"source": delegateUrl, "params": params});
    return overlay;
}
