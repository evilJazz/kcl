.pragma library

// roundToPrec is different from toFixed as it returns a Number
// instead of a String thus removing trailing zeros int the fractional part...
Number.prototype.roundToPrec = function(precision)
{
    var power = Math.pow(10, precision || 0);
    return Math.round(this * power) / power;
}

Number.prototype.fuzzyEqual = function(otherNumber)
{
    return Math.abs(this - otherNumber) <= 0.00001;
}
