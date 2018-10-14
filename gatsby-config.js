module.exports = {
  siteMetadata: {
    title: 'IPFS Uploader',
  },
  pathPrefix: `/extra`,
  plugins: [
    'gatsby-plugin-react-helmet',
    {
      resolve: 'gatsby-plugin-reason',
      options: {
        derivePathFromComponentName: true,
      },
    },
  ],
}
