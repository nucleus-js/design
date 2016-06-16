function assert(thing, message) {
  if (thing) return;
  throw new Error(message);
}

return assert
