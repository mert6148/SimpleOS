const franction = (numerator, denominator) => {
  if (denominator === 0) {
    throw new Error("Denominator cannot be zero");
  } else if (denominator < 0) {
    numerator = -numerator;
    denominator = -denominator;
  } else if (numerator === 0) {
    denominator = 1;
  }
}

module.exports = franction;

const gcd = (a, b) => {
  if (b === 0) {
    return a;
  } else {
    return gcd(b, a % b);
 }
}

const reduce = (numerator, denominator) => {
  const common = gcd(numerator, denominator);
  return [numerator / common, denominator / common];
}