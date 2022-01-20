import babel from '@rollup/plugin-babel'
import {terser} from "rollup-plugin-terser";

export default {
  input: './src/index.js',
  output: {
    file: './dist/index.js',
    format: 'umd',
    name: 'tinytex'
  },
  plugins: [
    babel({
      exclude: 'node_modules/**'
    })//,
    // terser()
  ]
}
