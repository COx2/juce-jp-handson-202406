import { useFrame } from '@react-three/fiber';
import { FC, useRef, useState } from 'react';
import type { Mesh } from 'three';

export const Cube: FC = (props: any) => {
    // Set up state for the hovered and active state
    const [hovered, setHover] = useState(false)
    const [active, setActive] = useState(false)
    // const [scale, setScale] = useState(1.0)
    // const [count, setCount] = useState(1)

    const cubeRef = useRef<Mesh>(null);
    useFrame(() => {
        const cube = cubeRef.current;
        if (!cube) return;
        cube.rotation.x += 0.01;
        cube.rotation.y += 0.01;
        if(active)
        {
            // setScale(Math.sin(count * 0.01) * 2.0)
            // setCount((prevCount) => prevCount + 1);
        }
    });
    return (
        <mesh 
            {...props}
            ref={cubeRef}
            scale={props.scale}
            onClick={() => setActive(!active)}
            onPointerOver={() => setHover(true)}
            onPointerOut={() => setHover(false)}
            >
            <boxGeometry args={[1, 1, 1]} />
            <meshStandardMaterial color={hovered ? 'yellow' : 'lightgreen'} />
        </mesh>
    );
};