.pragma library

String.prototype.trim = function() { return this.replace(/^\s+|\s+$/g, ''); }
String.prototype.removeEntities = function() { return this.replace(/&quot;/gi, "\"").replace(/&amp;/gi, "&"); }
String.prototype.startsWith = function(s) { return this.indexOf(s) === 0; }
String.prototype.endsWith = function (s) { return this.length >= s.length && this.substr(this.length - s.length) === s; }
String.prototype.replaceArgs = function(args)
{
    var re = new RegExp("%([0-9]+)", "g");

    var replacerCallback = function(match, p1, offset, string)
    {
        return args[parseInt(p1) - 1];
    }

    return this.replace(re, replacerCallback);
}

String.prototype.removeClasses = function(s)
{
    return this.replace(/style="[^"]*"|class="[^"]*"/gi, "");
}

String.prototype.lineFeedsToBreaks = function(s)
{
    return this.replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, '$1<br/>$2');
}

String.prototype.escape = function()
{
    return this.replace(/[\\"']/g, '\\$&').replace(/\u0000/g, '\\0');
}

String.prototype.escapeAttribute = function()
{
    return this.replace(/[\\"]/g, "&quot;");
}

String.prototype.escapeHtml = function()
{
    return this
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}

String.prototype.unescapeHtml = function()
{
    return this
        .replace(/&amp;/g, "&")
        .replace(/&lt;/g, "<")
        .replace(/&gt;/g, ">")
        .replace(/&quot;/g, "\"")
        .replace(/&#039;/g, "'");
}
