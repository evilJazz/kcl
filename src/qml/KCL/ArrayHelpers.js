.pragma library

Array.prototype.unique = function() {
    var a = [], l = this.length;
    for(var i=0; i<l; i++) {
        for(var j=i+1; j<l; j++)
            if (this[i] === this[j]) j = ++i;
        a.push(this[i]);
    }
    return a;
}

Array.prototype.remove = function(from, to) {
    var rest = this.slice((to || from) + 1 || this.length);
    this.length = from < 0 ? this.length + from : from;
    return this.push.apply(this, rest);
}
