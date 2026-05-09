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

const add = (numerator1, denominator1, numerator2, denominator2) => {
  const numerator = numerator1 * denominator2 + numerator2 * denominator1;
  const denominator = denominator1 * denominator2;
  return reduce(numerator, denominator);
}

const subtract = (numerator1, denominator1, numerator2, denominator2) => {
  const numerator = numerator1 * denominator2 - numerator2 * denominator1;
  const denominator = denominator1 * denominator2;
  return reduce(numerator, denominator);
}