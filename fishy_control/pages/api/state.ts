import type { NextApiRequest, NextApiResponse } from 'next'
import Cors from 'cors';
import type {requestData} from '../index';

let lastReceivedState:requestData = {Temperature:0 , Output:0};

// Initializing the cors middleware
// You can read more about the available options here: https://github.com/expressjs/cors#configuration-options
const cors = Cors({
  methods: ['POST', 'GET', 'HEAD'],
})

// Helper method to wait for a middleware to execute before continuing
// And to throw an error when an error happens in a middleware
function runMiddleware(
  req: NextApiRequest,
  res: NextApiResponse,
  fn: Function
) {
  return new Promise((resolve, reject) => {
    fn(req, res, (result: any) => {
      if (result instanceof Error) {
        return reject(result)
      }

      return resolve(result)
    })
  })
}

export default async function handler(
  req: NextApiRequest,
  res: NextApiResponse
) {
  // Run the middleware
  await runMiddleware(req, res, cors)
  const method = req.method;
  lastReceivedState = req.body;
 
  if (method == "GET") {
    
    res.status(200).json(lastReceivedState);
  }
  if (method == "POST") {
    console.log(req.body)
    lastReceivedState = req.body;
    res.send(200);
  }
}

