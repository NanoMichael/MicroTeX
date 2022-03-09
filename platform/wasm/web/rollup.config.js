import babel from '@rollup/plugin-babel';
import {terser} from "rollup-plugin-terser";
import {nodeResolve} from '@rollup/plugin-node-resolve';
import commonjs from "@rollup/plugin-commonjs";

export default [{
  input: './src/index.js',
  output: {
    file: './dist/microtex.js',
    format: 'esm',
    name: 'microtex'
  },
  plugins: [
    babel({
      exclude: 'node_modules/**'
    }),
    terser()
  ]
}, {
  input: './demo/index.js',
  output: {
    file: './demo-dist/index.js',
    format: 'esm',
    name: 'app',
    globals: {
      'ace-builds': 'ace',
      'ace': 'ace'
    }
  },
  plugins: [
    babel(),
    nodeResolve(),
    commonjs(),
    terser()
  ]
}];
