const path = require('node:path');
const webpack = require('webpack');

const TerserPlugin = require("terser-webpack-plugin");

// drop_console: process.env.FOOSTACK_DEV == 'yes' ? false : true,
const _terser_options = { // https://github.com/terser/terser/tree/v5.3.8?tab=readme-ov-file#minify-options
    parse: false,
    compress: {
        drop_console: process.env.FOOSTACK_DEV == 'yes' ? false : false,
        keep_infinity: true
    },
    mangle: false,
    format: {
        ascii_only: false,
        beautify: false,
        braces: false,
        comments: false,
        ecma: false,
        indent_level: 0,
        indent_start: 0,
        inline_script: false,
        keep_numbers: true,
        keep_quoted_props: true,
        max_line_len: false,
        preamble: null,
        quote_keys: false,
        quote_style: 0,
        preserve_annotations: false,
        safari10: false,
        semicolons: true,
        shebang: false,
        webkit: false,
        wrap_iife: false,
        wrap_func_args: false
    },
    sourceMap: false,
    ecma: undefined,
    keep_classnames: true,
    keep_fnames: true,
    ie8: false,
    module: false,
    nameCache: null,
    safari10: false,
    toplevel: false
}

module.exports = [
    {
        name: 'blobparts',
        target: 'web',
        mode: 'production',
        devtool: false,
        entry: {
            ws: './js/body/ws.js'
        },
        output: {
            path: path.resolve(__dirname),
            filename: '_generated/[name].bundle.js'
        },
        resolve: {
            fallback: {
                buffer: require.resolve('buffer/')
            }
        },
        optimization: {
            minimize: true,
            minimizer: [
                new TerserPlugin({
                    terserOptions: _terser_options,
                    extractComments: false
                })
            ]
        },
        plugins: [
          new webpack.ProvidePlugin({
              Buffer: ['buffer', 'Buffer'],
          })
        ]
    },
    {
        name: 'web',
        target: 'web',
        mode: 'production',
        devtool: false,
        entry: {
            main: './js/main.js',
            css: './css/main.scss'
        },
        output: {
            path: path.resolve(__dirname),
            filename: '_generated/[name].bundle.js'
        },
        module: {
            rules: [
                {
                    test: /\.scss$/,
                    type: "asset/resource",
                    generator: {
                        filename: '_generated/main.bundle.css',
                    },
                    use: [
                        {
                            loader: 'sass-loader',
                            options: {
                                sassOptions: {
                                    outputStyle: 'compressed'
                                }
                            }
                        }
                    ],
                }
            ]
        },
        resolve: {
            fallback: {
                buffer: require.resolve('buffer/')
            }
        },
        optimization: {
            minimize: true,
            minimizer: [
                new TerserPlugin({
                    terserOptions: _terser_options,
                    extractComments: false
                })
            ]
        },
        plugins: [
          new webpack.ProvidePlugin({
              Buffer: ['buffer', 'Buffer'],
          })
        ]
    }
];
