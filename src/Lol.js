import React, { Component } from 'react';
import ThreeBox from '3box'

class Lol extends Component {
  constructor(props) {
    super(props)
    this.state = {
      box: false
    }
  }

  render() {
    return (
      <div className="Lol">
        <header className="Lol-header">
          <button onClick={() => {
            ThreeBox.openBox(window.web3.eth.accounts[0], window.web3.currentProvider).then(box => {
              console.log(box)
              this.setState({ box })
            })
          }}>click me
          </button>
        </header>
      </div>
    );
  }
}

export default Lol;
