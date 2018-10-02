import React from 'react'
import Layout from '../layouts'
import App from '../components/App.re'

const IndexPage = () => <Layout>{data => <App data={data} />}</Layout>

export default IndexPage
