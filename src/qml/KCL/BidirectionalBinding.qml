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

QtObjectWithChildren {
    id: entangler

    property alias leftTarget: left.target
    property alias rightTarget: right.target

    property alias leftPropertyName: left.propertyName
    property alias rightPropertyName: right.propertyName

    property alias leftValid: left.valid
    property alias rightValid: right.valid

    property bool valid: leftValid && rightValid

    property bool enabled: true

    function copyLeftToRight()
    {
        if (!entangler.enabled) return;

        if (entangler._settingValue) return;
        if (entangler.valid)
        {
            var value = convertLeftToRight(leftTarget[leftPropertyName]);
            if (rightTarget[rightPropertyName] !== value)
            {
                entangler._settingValue = true;
                //console.log("COPIED! " + rightTarget + "." + rightPropertyName + " = " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
                rightTarget[rightPropertyName] = value;
                entangler._settingValue = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + rightTarget + "." + rightPropertyName + " == " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    function copyRightToLeft()
    {
        if (!entangler.enabled) return;

        if (entangler._settingValue) return;
        if (entangler.valid)
        {
            var value = convertRightToLeft(rightTarget[rightPropertyName]);
            if (leftTarget[leftPropertyName] !== value)
            {
                entangler._settingValue = true;
                //console.log("COPIED! " + leftTarget + "." + leftPropertyName + " = " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
                leftTarget[leftPropertyName] = value;
                entangler._settingValue = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + leftTarget + "." + leftPropertyName + " == " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    /* Override these methods if you want to customize conversion, e.g. change units or do other calculations. */

    function convertRightToLeft(right)
    {
        return right;
    }

    function convertLeftToRight(left)
    {
        return left;
    }

    /* Private section */

    ValueChangeWatcher {
        id: left
        enabled: entangler.enabled
        onValueChanged: entangler.copyLeftToRight()
    }

    ValueChangeWatcher {
        id: right
        enabled: entangler.enabled
        onValueChanged: entangler.copyRightToLeft()
    }

    property bool _settingValue: true
    Component.onCompleted: _settingValue = false
}
