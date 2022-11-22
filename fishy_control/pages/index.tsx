import * as React from 'react';
import { Card } from '@mui/material';
import { Stack } from '@mui/system';
import Menu from '../components/menu'
import useSwr from 'swr';
import axios from 'axios';
import CircularIndeterminate from '../components/CircularIndeterminate';

export type requestData = {
    Temperature: number,
    Output: number
};

let tempOut:requestData;

export default function Dashboard() {
  const [isLoading, setIsLoading] = React.useState(false);
  const [temp, setTemp] = React.useState(0);
  const [out, setOut] = React.useState(0);
  
  async function getData() {
    try {
      // 👇️ const data: GetUsersResponse
      const { data, status } = await axios.get<requestData>(
        'https://fishy-control.vercel.app/api/data',
        {
          headers: {
            Accept: 'application/json',
          },
        },
      );
  
      console.log(JSON.stringify(data, null, 4));
  
      // 👇️ "response status is: 200"
      console.log('response status is: ', status);
  
      tempOut = data;
      setTemp(tempOut.Temperature);
      setOut(tempOut.Output);
    } catch (error) {
      if (axios.isAxiosError(error)) {
        console.log('error message: ', error.message);
        return error.message;
      } else {
        console.log('unexpected error: ', error);
        return 'An unexpected error occurred';
      }
    }
  }


  React.useEffect(() => {
    const interval = setInterval(() => {
      getData();
      
    }, 5000);
    return () => clearInterval(interval);
  }, []);

  //if (error) return <div>Failed to load data</div>
  if (isLoading) return <CircularIndeterminate/>

  return (
    <>
    <Menu/>
    <Stack
      direction="column"
      justifyContent="center"
      alignItems="center"
      spacing={3}
    > 
      <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700, width: 400, height:70 , p:2 , m:2}}>
        Dashboard de Controle
      </Card>
      <Stack
      direction="row"
      justifyContent="center"
      alignItems="center"
      spacing={3}
      >
        <Card sx={{ fontSize: 20,textAlign: 'center', fontWeight: 400,width: 200, height:70 , p:2 , m:2}}>
          Termopar tipo K
        </Card>
        <Card sx={{ fontSize: 20,textAlign: 'center', fontWeight: 400,width: 200, height:70 , p:2 , m:2}}>
          PID Output
        </Card>
      </Stack>
      
      <Stack
      direction="row"
      justifyContent="center"
      alignItems="center"
      spacing={3}
      >
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {temp} C
        </Card>
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {out}
        </Card>
      </Stack>
    </Stack>  
    </>
  );
}