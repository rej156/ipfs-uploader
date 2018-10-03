const Uppy = require('@uppy/core')
const { Plugin } = require('@uppy/core')
const Dashboard = require('@uppy/dashboard')
const settle = require('@uppy/utils/lib/settle')
const IPFS = require('ipfs-api')

const ipfs = new IPFS({ host: 'ipfs.infura.io', port: 5001, protocol: 'https' })
// const limitPromises = require('@uppy/utils/lib/limitPromises')

class MyPlugin extends Plugin {
  constructor(uppy, opts) {
    super(uppy, opts)
    this.id = 'IPFSUploader'
    this.type = 'uploader'
    this.title = 'IPFSUploader'
    this.limitUploads = fn => fn
  }

  prepareUpload = fileIDs => {
    return Promise.resolve()
  }

  uploadFiles = files => {
    const actions = files.map((file, i) => {
      const current = parseInt(i, 10) + 1
      const total = files.length

      console.log(this)

      if (file.error) {
        return () => Promise.reject(new Error(file.error))
      } else if (file.isRemote) {
        // We emit upload-started here, so that it's also emitted for files
        // that have to wait due to the `limit` option.
        this.uppy.emit('upload-started', file)
        return this.uploadRemote.bind(this, file, current, total)
      } else {
        console.log(this)
        this.uppy.emit('upload-started', file)
        return this.upload.bind(this, file, current, total)
      }
    })

    const promises = actions.map(action => {
      const limitedAction = this.limitUploads(action)
      return limitedAction()
    })

    return settle(promises)
  }

  upload(file, current, total) {
    return new Promise((resolve, reject) => {
      this.uppy.log(`uploading ${current} of ${total}`)
      console.log(file)
      const reader = new FileReader()

      console.log(reader)
      reader.readAsArrayBuffer(file.data)
      const saveToIpfs = reader => {
        const buffer = Buffer.from(reader.result)
        ipfs
          .add(buffer, {
            progress: prog => console.log(`received: ${prog}`),
          })
          .then(res => {
            console.log(res)
            let ipfsFile = res[0]
            console.log(ipfsFile)
            this.uppy.log(`IPFS Upload of file ${ipfsFile.id} finished.`)

            const response = {
              status: 200,
            }

            this.uppy.setFileState(file.id, { response })

            this.uppy.on('file-removed', removedFile => {})

            this.uppy.on('upload-cancel', fileID => {})

            this.uppy.on('cancel-all', () => {})
            resolve(ipfsFile)
          })
          .catch(err => {
            console.error(err)
            this.uppy.emit('upload-error', file, err)
            reject(err)
          })
      }
      reader.onloadend = () => saveToIpfs(reader)
    })
  }

  ipfsUploader = fileIDS => {
    return Promise.resolve()
  }

  install() {
    this.uppy.addUploader(fileIDs => {
      console.log(this)
      if (fileIDs.length === 0) {
        this.uppy.log('[XHRUpload] No files to upload!')
        return Promise.resolve()
      }

      this.uppy.log('[XHRUpload] Uploading...')
      const files = fileIDs.map(fileID => this.uppy.getFile(fileID))

      if (this.opts.bundle) {
        return this.uploadBundle(files)
      }

      return this.uploadFiles(files).then(() => null)
    })
    // this.uppy.addUploader(this.ipfsUploader)
  }

  uninstall() {
    this.uppy.removeUploader(this.handleUpload)
  }
}

export default () => {
  const uppy = Uppy()
    .use(Dashboard, {
      trigger: '#select-files',
    })
    .use(MyPlugin)

  uppy.on('complete', result => {
    console.log(result)
    console.log(
      `Upload complete! We’ve uploaded these files: ${result.successful}`
    )
  })
}
