import * as React from 'react';
import { Card } from '@mui/material';
import { Stack } from '@mui/system';
import Menu from '../components/menu'
import useSwr from 'swr';
import CircularIndeterminate from '../components/CircularIndeterminate';
export type requestData = {
    Temperature: number,
    Output: number
};

const fetcher = (url: string) => fetch(url).then((res) => res.json())

export default function Dashboard() {
  const { data, error } = useSwr<requestData>('/api/data', fetcher,{ refreshInterval: 1000 })

  if (error) return <div>Failed to load data</div>
  if (!data) return <CircularIndeterminate/>

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
          {data.Temperature} C
        </Card>
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {data.Output}
        </Card>
      </Stack>
    </Stack>  
    </>
  );
}