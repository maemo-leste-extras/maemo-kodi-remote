import QtQuick 2.0
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

StackView {
    // Usage: 
    //   - set `initialItem` during initialization
    //   - call e.g `stackReplace(component, 'rightToLeft', duration)` for a transition
    //
    // Transitions: 
    //   - fade
    //   - rightToLeft
    //   - leftToRight
    //   - topToBottom
    //   - bottomToTop
    //
    // onPageBegin() is called on child frame at the beginning of the transition
    // onPageComplete() is called on child frame at the beginning of the transition

    id: root
    property int duration: 500

    function callPageComplete() {
        if(root.currentItem === null) return;
        if(typeof root.currentItem.onPageCompleted === "function")
            root.currentItem.onPageCompleted();
    }

    function callPageBegin() {
        if(root.currentItem === null) return;
        if(typeof root.currentItem.onPageBegin === "function")
            root.currentItem.onPageBegin();
    }

    function stackReplace(target, transitionType, duration) {
        var ttype = transitionType;

        // milliseconds
        var _duration = 0;
        if(typeof duration !== "undefined") {
            _duration = duration
        } else {
            _duration = 500;
        }

        // reset
        replaceEnterAnimationOpacity.duration = 0;
        replaceExitAnimationOpacity.duration = 0;
        replaceEnterAnimationX.duration = 0;
        replaceEnterAnimationX.from = 0;
        replaceEnterAnimationX.to = 0;
        replaceExitAnimationX.duration = 0;
        replaceExitAnimationX.from = 0;
        replaceExitAnimationX.to = 0;
        replaceEnterAnimationY.duration = 0;
        replaceEnterAnimationY.from = 0;
        replaceEnterAnimationY.to = 0;
        replaceExitAnimationY.duration = 0;
        replaceExitAnimationY.from = 0;
        replaceExitAnimationY.to = 0;
        var sWidth = root.width;
        var sHeight = root.height;

        callPageBegin()

        switch (ttype) {
            case 'fade': {
                replaceEnterAnimationOpacity.duration = _duration;
                replaceExitAnimationOpacity.duration = _duration;
                break;
            }
            case 'rightToLeft': {
                replaceEnterAnimationX.duration = _duration;
                replaceExitAnimationX.duration = _duration;
                
                replaceEnterAnimationX.from = sWidth;
                replaceEnterAnimationX.to = 0;
                
                replaceExitAnimationX.from = 0;
                replaceExitAnimationX.to = -sWidth;
                break;
            }
            case 'leftToRight': {
                replaceEnterAnimationX.duration = _duration;
                replaceExitAnimationX.duration = _duration;
                
                replaceEnterAnimationX.from = -sWidth;
                replaceEnterAnimationX.to = 0;
                
                replaceExitAnimationX.from = 0;
                replaceExitAnimationX.to = sWidth;
                break;
            }
            case 'topToBottom': {
                replaceEnterAnimationY.duration = _duration;
                replaceExitAnimationY.duration = _duration;
                
                replaceEnterAnimationY.from = -sHeight;
                replaceEnterAnimationY.to = 0;
                
                replaceExitAnimationY.from = 0;
                replaceExitAnimationY.to = sHeight;
                break;
            }
            case 'bottomToTop': {
                replaceEnterAnimationY.duration = _duration;
                replaceExitAnimationY.duration = _duration;
                
                replaceEnterAnimationY.from = sHeight;
                replaceEnterAnimationY.to = 0;
                
                replaceExitAnimationY.from = 0;
                replaceExitAnimationY.to = -sHeight;
                break;
            }
        }

        root.replace(target);

        ctx.singleShot(_duration, function () {
            callPageComplete();
        });
    }

    replaceEnter: Transition {
        PropertyAnimation {
            id: replaceEnterAnimationOpacity
            property: "opacity"
            from: 0
            to: 1
            duration: 0
            easing.type: Easing.OutQuart
        }
        PropertyAnimation {
            id: replaceEnterAnimationX
            property: "x"
            from: 0
            to: 0
            duration: 0
            easing.type: Easing.OutQuart
        }
        PropertyAnimation {
            id: replaceEnterAnimationY
            property: "y"
            from: 0
            to: 0
            duration: 0
            easing.type: Easing.OutQuart
        }
    }

    replaceExit: Transition {
        PropertyAnimation {
            id: replaceExitAnimationOpacity
            property: "opacity"
            from: 1
            to: 1
            duration: 0
            easing.type: Easing.OutQuart
        }
        PropertyAnimation {
            id: replaceExitAnimationX
            property: "x"
            from: 0
            to: 0
            duration: 0
            easing.type: Easing.OutQuart
        }
        PropertyAnimation {
            id: replaceExitAnimationY
            property: "y"
            from: 0
            to: 0
            duration: 0
            easing.type: Easing.OutQuart
        }
    }

    Component.onCompleted: root.callPageComplete();
}