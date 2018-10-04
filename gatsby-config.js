module.exports = {
  siteMetadata: {
    title: 'Gatsby v2 ReasonML',
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
