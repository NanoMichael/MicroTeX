import babel from '@rollup/plugin-babel';
import {terser} from "rollup-plugin-terser";
import {nodeResolve} from '@rollup/plugin-node-resolve';
import commonjs from "@rollup/plugin-commonjs";

export default {
  input: './example/index.js',
  output: {
    file: './example-dist/index.js',
    format: 'esm',
    name: 'app',
    globals: {
      'ace-builds': 'ace',
      'ace': 'ace'
    },
    sourcemap: true
  },
  plugins: [
    babel(),
    nodeResolve(),
    commonjs(),
    terser()
  ]
};
