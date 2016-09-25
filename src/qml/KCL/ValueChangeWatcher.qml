/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

QtObject {
    id: watcher

    property bool enabled: true

    property QtObject target
    property string propertyName

    property bool valid: _evaluateValid(target, propertyName)

    signal valueChanged(variant oldValue, variant newValue);

    /* Private section */

    onTargetChanged: _updateBinding()
    onPropertyNameChanged: _updateBinding()

    property QtObject _boundTarget
    property string _boundPropertyName

    property variant _oldValue
    property bool _settingValue

    function _updateBinding()
    {
        if (_evaluateValid(_boundTarget, _boundPropertyName))
            _boundTarget[_boundPropertyName + "Changed"].disconnect(watcher._boundValueChanged);

        _boundTarget = target;
        _boundPropertyName = propertyName;
        _oldValue = null;

        if (_evaluateValid(_boundTarget, _boundPropertyName))
        {
            var bindSignal = _boundPropertyName + "Changed";

            if (!_boundTarget.hasOwnProperty(bindSignal))
            {
                console.log("Target " + _boundTarget + " does not implement a signal named " + bindSignal + " for property " + _boundPropertyName);
                return;
            }

            _boundTarget[bindSignal].connect(watcher._boundValueChanged);
        }
    }

    function _evaluateValid(target, propertyName)
    {
        var targetValid = target !== null && typeof(target) !== "undefined";
        var propertyValid = targetValid && target.hasOwnProperty(propertyName);

        if (targetValid && !propertyValid)
            console.log("Target " + target + " does not contain property " + propertyName + "!");

        return propertyValid;
    }

    function _boundValueChanged()
    {
        if (!enabled) return;
        if (_settingValue) return;

        var newValue = _boundTarget[_boundPropertyName];
        if (newValue !== _oldValue)
        {
            try
            {
                _settingValue = true;
                //console.log("VALUE CHANGED! target: " + target + " property: " + propertyName + " newValue: " + newValue + " oldValue: " + _oldValue);
                var oldValue = _oldValue;
                _oldValue = newValue; // set oldValue before descending into signal slots...
                valueChanged(oldValue, newValue);
            }
            finally
            {
                _settingValue = false;
            }
        }
        else
        {
            //console.log("VALUE UNCHANGED! target: " + target + " property: " + propertyName + " newValue: " + newValue + " oldValue: " + _oldValue);
        }
    }
}
