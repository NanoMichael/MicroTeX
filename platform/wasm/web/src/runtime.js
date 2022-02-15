'use strict'

import Module from "../gen/tinytex-wasm";

let runtime = {};

function initRuntime() {
  return Module().then(r => {
    runtime = r;
    return runtime;
  });
}

export {runtime, initRuntime};
