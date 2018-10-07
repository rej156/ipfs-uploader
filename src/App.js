import React, { Component } from 'react';
import logo from './logo.svg';
import IPFSUploader from './App.bs'
import './App.css';

class App extends Component {
  

  render() {
    return (
      <div className="App">
        <link href="https://transloadit.edgly.net/releases/uppy/v0.27.5/dist/uppy.min.css" rel="stylesheet"></link>
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <IPFSUploader />
        </header>
      </div>
    );
  }
}

export default App;
