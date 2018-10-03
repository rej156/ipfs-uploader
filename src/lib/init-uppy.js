const Uppy = require('@uppy/core')
const { Plugin } = require('@uppy/core')
const Dashboard = require('@uppy/dashboard')
const settle = require('@uppy/utils/lib/settle')
const IPFS = require('ipfs-api')

const ipfs = new IPFS({ host: 'ipfs.infura.io', port: 5001, protocol: 'https' })
// const limitPromises = require('@uppy/utils/lib/limitPromises')

const uppyConfig = {
  debug: true,
  autoProceed: true,
  restrictions: {
    maxNumberOfFiles: 1,
    minNumberOfFiles: 1,
    allowedFileTypes: ['image/png', 'image/jpeg', 'image/jpg'],
  },
}

class IPFSUploader extends Plugin {
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

      if (file.error) {
        return () => Promise.reject(new Error(file.error))
      } else {
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

  createProgressTimeout(timeout, timeoutHandler) {
    const uppy = this.uppy
    let isDone = false

    function onTimedOut() {
      uppy.log(`[XHRUpload] timed out`)
      const error = new Error(`timedOut: ${Math.ceil(timeout / 1000)}`)
      timeoutHandler(error)
    }

    let aliveTimer = null
    function progress() {
      // Some browsers fire another progress event when the upload is
      // cancelled, so we have to ignore progress after the timer was
      // told to stop.
      if (isDone) return

      if (timeout > 0) {
        if (aliveTimer) clearTimeout(aliveTimer)
        aliveTimer = setTimeout(onTimedOut, timeout)
      }
    }

    function done() {
      uppy.log(`[IPFSUpload] timer done`)
      if (aliveTimer) {
        clearTimeout(aliveTimer)
        aliveTimer = null
      }
      isDone = true
    }

    return {
      progress,
      done,
    }
  }

  upload(file, current, total) {
    return new Promise((resolve, reject) => {
      this.uppy.log(`uploading ${current} of ${total}`)
      console.log(file)
      const reader = new FileReader()
      reader.readAsArrayBuffer(file.data)

      const timer = this.createProgressTimeout(10000, error => {
        this.uppy.emit('upload-error', file, error)
        reject(error)
      })
      this.uppy.log(`[IPFSUpload] ${file.id} started`)
      timer.progress()

      const saveToIpfs = reader => {
        const buffer = Buffer.from(reader.result)
        ipfs
          .add(buffer, {
            progress: progress => console.log(`received: ${progress}`),
          })
          .then(res => {
            console.log(res)
            let ipfsFile = res[0]
            const body = ipfsFile
            console.log(ipfsFile)
            this.uppy.log(`IPFS Upload of file ${file.id} finished.`)

            const uploadURL = `https://gateway.ipfs.io/ipfs/${ipfsFile.hash}`

            const response = {
              status: 200,
              body,
              uploadURL,
            }

            this.uppy.setFileState(file.id, { response })

            this.uppy.on('file-removed', removedFile => {})

            this.uppy.on('upload-cancel', fileID => {})

            this.uppy.on('cancel-all', () => {})
            this.uppy.emit('upload-progress', file, {
              uploader: this,
              bytesUploaded: file.size,
              bytesTotal: file.size,
            })

            this.uppy.emit('upload-success', file, body, uploadURL)
            timer.done()
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
  const uppy = Uppy(uppyConfig)
    .use(Dashboard, {
      trigger: '#select-files',
      closeModalOnClickOutside: true,
      showProgressDetails: false,
      proudlyDisplayPoweredByUppy: false,
    })
    .use(IPFSUploader)

  uppy.on('complete', result => {
    console.log(result)
    console.log(
      `Upload complete! We’ve uploaded these files: ${JSON.stringify(
        result.successful
      )}`
    )
    setTimeout(() => {
      uppy.getPlugin('Dashboard').closeModal()
      uppy.close()
    }, 400)
  })

  uppy.run()
  uppy.getPlugin('Dashboard').openModal()
  return uppy
}
