.pragma library

String.prototype.trim = function() { return this.replace(/^\s+|\s+$/g, ''); }
String.prototype.removeEntities = function() { return this.replace(/&quot;/gi, "\"").replace(/&amp;/gi, "&"); }
String.prototype.startsWith = function(s) { return this.indexOf(s) === 0; }
String.prototype.endsWith = function (s) { return this.length >= s.length && this.substr(this.length - s.length) === s; }
String.prototype.replaceArgs = function(args)
{
    var str = this;

    for (var i = 0; i < args.length; ++i)
        str = str.replace(eval("/%" + (i + 1 ) + "/"), args[i]);

    return str;
}

String.prototype.removeClasses = function(s)
{
    return this.replace(/style="[^"]*"|class="[^"]*"/gi, "");
}

String.prototype.lineFeedsToBreaks = function(s)
{
    return this.replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, '$1<br/>$2');
}
