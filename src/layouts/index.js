import React from 'react'
import PropTypes from 'prop-types'
import { withPrefix, StaticQuery, graphql } from 'gatsby'
import { Helmet } from 'react-helmet'

const Layout = ({ children }) => (
  <StaticQuery
    query={graphql`
      query SiteTitleQuery {
        site {
          siteMetadata {
            title
          }
        }
      }
    `}
    render={data => (
      <div>
        <Helmet>
          <script src={withPrefix('/3box.min.js')} type="text/javascript" />
          <link
            rel="stylesheet"
            href="https://fonts.googleapis.com/css?family=Roboto:300,400,500"
          />
          <link
            rel="stylesheet"
            href="https://fonts.googleapis.com/icon?family=Material+Icons"
          />
        </Helmet>
        {children(data)}
      </div>
    )}
  />
)

Layout.propTypes = {
  children: PropTypes.func.isRequired,
}

export default Layout
