import babel from "@rollup/plugin-babel";
import {terser} from "rollup-plugin-terser";

export default {
  input: './lib/index.js',
  output: {
    file: './dist/microtex.js',
    format: 'esm',
    name: 'microtex',
    sourcemap: true
  },
  plugins: [
    babel({exclude: 'node_modules/**'}),
    terser()
  ]
};
