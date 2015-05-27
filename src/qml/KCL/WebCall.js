/***************************************************************************
 *   Copyright (C) 2011-2012 Andre Beckedorf                               *
 *                           <evilJazz _AT_ katastrophos _DOT_ net>        *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

//.import QtQuick.LocalStorage 2.0 as SQL //@QtQuick2

var debug = false;
var cCacheExpiryTimeInMinutes = 2 * 7 * 1440; // two weeks
var cCacheMaxEntriesToKeep = 1000;

/* useCache constants */
var NoCache = false;
var UseCache = true;
var UpdateCache = 2;

function getElementByTagName(element, nodeName)
{
    var childNodes = element.childNodes;
    for (var i = 0; i < childNodes.length; ++i)
    {
        /*
        console.log("childNodes[" + i + "].nodeName = " + childNodes[i].nodeName);
        console.log("childNodes[" + i + "].tagName = " + childNodes[i].tagName);
        console.log("childNodes[" + i + "].nodeValue = " + childNodes[i].nodeValue);
        console.log("childNodes[" + i + "].wholeText = " + childNodes[i].wholeText);
        console.log("childNodes[" + i + "].innerHTML = " + childNodes[i].innerHTML);
        */

        if (childNodes[i].nodeName == nodeName)
        {
            //console.log("Found at index " + i + ": " + childNodes[i].nodeName);
            return childNodes[i];
        }
    }

    //console.log("Nothing was found.");
    return null;
}

function getTextByTagName(element, nodeName)
{
    return getElementByTagName(element, nodeName).childNodes[0].wholeText;
}

function insertIntoCache(url, params, method, response)
{
    var paramsJSON = JSON.stringify(params);
    var db = openDatabaseSync("WebCall", "1.0", "WebCall Cache Database", 1000000); //@QtQuick1
    //var db = SQL.LocalStorage.openDatabaseSync("WebCall", "1.0", "WebCall Cache Database", 1000000); //@QtQuick2

    for (var i = 0; i < 2; ++i)
    try
    {
        db.transaction(
            function(tx)
            {
                tx.executeSql("DELETE FROM callcache WHERE id IN (SELECT id FROM callCache ORDER BY timestamp ASC LIMIT (SELECT MAX(0, COUNT() - ?) FROM callcache));", [cCacheMaxEntriesToKeep - 1]);
                tx.executeSql("DELETE FROM callcache WHERE datetime(timestamp, 'unixepoch') < datetime('now', ?);", ["- " + cCacheExpiryTimeInMinutes + " minutes"]);
                tx.executeSql("DELETE FROM callCache WHERE `method` = ? AND `url` = ? AND `params` = ?;", [method, url, paramsJSON]);
                tx.executeSql("INSERT INTO callCache VALUES(NULL, ?, ?, ?, ?, strftime('%s', 'now'));", [method, url, paramsJSON, response]);

                if (debug) console.log("Inserted into cache...");
            }
        );
        break;
    }
    catch (e)
    {
        db.transaction(
            function(tx)
            {
                try
                {
                    tx.executeSql("DROP TABLE IF EXISTS callCache;");
                    tx.executeSql("CREATE TABLE callCache(`id` INTEGER PRIMARY KEY, `method` TEXT, `url` TEXT, `params` TEXT, `response` TEXT, `timestamp` INTEGER);");
                    tx.executeSql("CREATE INDEX callCacheKeyLookupIndex ON callCache(`method`, `url`, `params`);");
                    tx.executeSql("CREATE INDEX callCacheTimeLookupIndex ON callCache(`timestamp`);");
                }
                catch (e)
                {
                    console.log("Database error: " + e);
                }
            }
        );
    }
}

function getFromCache(url, params, method)
{
    var paramsJSON = JSON.stringify(params);
    var db = openDatabaseSync("WebCall", "1.0", "WebCall Cache Database", 1000000); //@QtQuick1
    //var db = SQL.LocalStorage.openDatabaseSync("WebCall", "1.0", "WebCall Cache Database", 1000000); //@QtQuick2

    var response = false;

    try
    {
        db.readTransaction(
            function(tx)
            {
                var results = tx.executeSql("SELECT * FROM callCache WHERE `method` = ? AND `url` = ? AND `params` = ?;", [method, url, paramsJSON]);

                if (debug) console.log("WebCall Cache results: " + JSON.stringify(results));

                if (results.rows.length > 0)
                    response = results.rows.item(0);
            }
        );
    }
    catch (e)
    {
    }

    return response;
}

function get(url, params, callbackObject, useCache)
{
    webCall(url, params, "GET", callbackObject, useCache);
}

function getBinary(url, params, callbackObject, useCache)
{
    webCall(url, params, "GETBINARY", callbackObject, useCache);
}

function post(url, params, callbackObject, useCache)
{
    webCall(url, params, "POST", callbackObject, useCache);
}

function postBinary(url, params, callbackObject, useCache)
{
    webCall(url, params, "POSTBINARY", callbackObject, useCache);
}

function webCall(url, params, action, callbackObject, useCache)
{
    var callurl, jsonResponse, successCallbackFunction, failureCallbackFunction;

    if (typeof(callbackObject) !== 'undefined' && typeof(callbackObject.success) !== 'undefined')
        successCallbackFunction = callbackObject.success;

    if (typeof(callbackObject) !== 'undefined' && typeof(callbackObject.failure) !== 'undefined')
        failureCallbackFunction = callbackObject.failure;

    if (typeof(action) == 'undefined' || action === null)
        action = "GET";

    if (useCache === UseCache && typeof(successCallbackFunction) !== "undefined")
    {
        var cachedData = getFromCache(url, params, action);
        if (cachedData !== false)
        {
            cachedData.responseXML = null;
            cachedData.responseText = cachedData.response;
            returnResponseToCallbackFunction(successCallbackFunction, cachedData);
            return;
        }
    }

    if (!NetworkUtils.isOnline)
    {
        if (debug) console.log("No internet connection, will not do anything.");

        if (typeof(failureCallbackFunction) !== "undefined")
        {
            var result = {};
            result.status = 0;
            result.statusText = "No internet connection";

            failureCallbackFunction(result);
        }

        return;
    }

    var paramArray = [];

    for (var param in params)
        paramArray.push(param + "=" + encodeURIComponent(params[param]));

    var ueParams = paramArray.join('&');

    if (action === "GETBINARY" || action === "POSTBINARY")
    {
        var bfl = Qt.createQmlObject('import KCL 1.0; BinaryFileDownloader { autoDelete: true; }',  Qt.hasOwnProperty("application") ? Qt.application : app);

        if (typeof(successCallbackFunction) !== "undefined")
            bfl.downloaded.connect(function(data)
            {
                //if (debug) console.log("bfl: " + JSON.stringify(bfl));

                if (typeof(useCache) !== "undefined" && useCache !== NoCache)
                    insertIntoCache(url, params, action, data);

                if (typeof(successCallbackFunction) !== "undefined")
                    returnResponseToCallbackFunction(successCallbackFunction, bfl);

                //bfl.destroy(); // not necessary because we are autoDeleting above...
            });

        if (typeof(failureCallbackFunction) !== "undefined")
            bfl.error.connect(function(errorCode, errorText)
            {
                var result = {};
                result.status = errorCode;
                result.statusText = errorText;
                failureCallbackFunction(result);
            });

        if (action === "GETBINARY")
        {
            if (ueParams.length > 0)
                callurl = url + '?' + ueParams;
            else
                callurl = url;

            bfl.get(callurl);
        }
        else if (action === "POSTBINARY")
        {
            callurl = url;

            bfl.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            bfl.setRequestHeader("Content-length", ueParams.length);
            bfl.setRequestHeader("Connection", "close");

            bfl.post(callurl, ueParams);
        }
    }
    else
    {
        var xhr = new XMLHttpRequest;

        if (action === "GET")
        {
            callurl = url + '?' + ueParams;
            if (debug) console.log("GET callurl: " + callurl);
            xhr.open("GET", callurl);
        }
        else
        {
            callurl = url;
            if (debug) console.log("POST callurl: " + callurl);
            if (debug) console.log("POST ueParams: " + ueParams);
            xhr.open("POST", callurl);
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.setRequestHeader("Content-length", ueParams.length);
            xhr.setRequestHeader("Connection", "close");
        }

        xhr.onreadystatechange = function()
        {
            //console.log("xhr: " + JSON.stringify(xhr));
            //console.log("xhr.readyState: " + xhr.readyState);

            if (xhr.readyState === XMLHttpRequest.DONE)
            {
                if (xhr.status != 200 || xhr.statusText != "OK")
                {
                    if (debug)
                    {
                        console.log("FAILURE!!!");
                        console.log("xhr.status: " + xhr.status);
                        console.log("xhr.statusText: " + xhr.statusText);
                    }

                    if (typeof(failureCallbackFunction) !== "undefined")
                    {
                        var result = {};
                        result.status = xhr.status;
                        result.statusText = xhr.statusText;

                        failureCallbackFunction(result);
                    }
                }
                else
                {
                    if (debug)
                    {
                        console.log("SUCCESS!!!");
                        console.log("ResponseText returned: " + xhr.responseText);
                        console.log("ResponseXML returned: " + xhr.responseXML);
                    }

                    if (typeof(useCache) !== "undefined" && useCache !== NoCache)
                        insertIntoCache(url, params, action, xhr.responseText);

                    if (typeof(successCallbackFunction) !== "undefined")
                        returnResponseToCallbackFunction(successCallbackFunction, xhr);
                }
            }
        }

        if (action === "GET")
            xhr.send();
        else
            xhr.send(ueParams);
    }
};

function returnResponseToCallbackFunction(successCallbackFunction, xhr)
{
    var data;

    if ("responseXML" in xhr && xhr.responseXML !== null)
    {
        data = xhr.responseXML;
    }
    else
    {
        if ("responseText" in xhr)
            data = xhr.responseText;
        else
            data = xhr.downloadedData;

        try
        {
            data = JSON.parse(data);
            if (debug) console.log("Parsed JSON content: " + JSON.stringify(data));
        }
        catch (e) {
            if (debug) console.log(e.message)
        }
    }

    successCallbackFunction(data);
}
