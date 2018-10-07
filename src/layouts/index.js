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
          <script src={'/3box.js'} type="text/javascript" />
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
