import babel from '@rollup/plugin-babel'

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
    })
  ]
}
